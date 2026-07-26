use midir::{MidiInput, MidiInputConnection};
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::{Arc, Mutex};
use std::thread;
use std::time::Duration;

const NOTE_NAMES: &[&str] = &[
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B",
];

const SCALE_NOTES: &[u8] = &[
    48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 71, 69, 67, 65, 64, 62, 60, 59, 57,
    55, 53, 52, 50, 48,
];

pub type EventSender = Arc<dyn Fn(String) + Send + Sync>;

struct ConnectState {
    midi_in: Option<MidiInput>,
    port_index: u32,
}

static CONNECT_STATE: Mutex<Option<ConnectState>> = Mutex::new(None);
static CONNECTION: Mutex<Option<MidiInputConnection<()>>> = Mutex::new(None);
static SIM_STOP: Mutex<Option<Arc<AtomicBool>>> = Mutex::new(None);

fn simulation_setting() -> Option<String> {
    let val = std::env::var("SIMULATION").ok().or_else(|| {
        #[cfg(target_os = "android")]
        {
            android_system_property("debug.nano.sim")
        }
        #[cfg(not(target_os = "android"))]
        {
            None
        }
    })?;
    if val.is_empty() {
        return None;
    }
    if val == "infinity" || val.parse::<u32>().is_ok() {
        Some(val)
    } else {
        None
    }
}

#[cfg(target_os = "android")]
fn android_system_property(name: &str) -> Option<String> {
    use std::ffi::CStr;
    let c_name = std::ffi::CString::new(name).ok()?;
    let mut buf = [0u8; 256];
    let len = unsafe {
        libc::__system_property_get(
            c_name.as_ptr() as *const libc::c_char,
            buf.as_mut_ptr() as *mut libc::c_char,
        )
    };
    if len > 0 {
        let val = unsafe { CStr::from_ptr(buf.as_ptr() as *const libc::c_char) };
        val.to_str().ok().map(|s| s.to_string())
    } else {
        None
    }
}

fn simulation_enabled() -> bool {
    simulation_setting().is_some()
}

fn simulation_infinite() -> bool {
    matches!(simulation_setting().as_deref(), Some("infinity"))
}

fn simulation_loop_count() -> u32 {
    simulation_setting()
        .and_then(|s| s.parse::<u32>().ok())
        .unwrap_or(0)
}

fn format_midi_event(bytes: &[u8]) -> Option<String> {
    if bytes.len() < 3 {
        return None;
    }
    let status = bytes[0] & 0xF0;
    let note = bytes[1];
    let velocity = bytes[2];

    let event_type = if status == 0x90 && velocity > 0 {
        "NOTE_ON"
    } else {
        "NOTE_OFF"
    };

    let raw = bytes
        .iter()
        .map(|b| format!("{b:02X}"))
        .collect::<Vec<_>>()
        .join(" ");

    let note_idx = (note % 12) as usize;
    let note_name = NOTE_NAMES[note_idx];
    let octave = (note / 12) as i32 - 1;
    let name = format!("{note_name}{octave}");

    Some(format!("{event_type} {raw} {name}"))
}

pub fn list_midi_ports() -> Vec<String> {
    if simulation_enabled() {
        return vec!["Simulated MIDI Device".to_string()];
    }
    let Ok(midi_in) = MidiInput::new("nano-list") else {
        return vec![];
    };
    midi_in
        .ports()
        .iter()
        .filter_map(|p| midi_in.port_name(p).ok())
        .collect()
}

pub fn connect_midi(port_index: u32) -> Result<String, String> {
    let ports = list_midi_ports();
    let port_name = ports
        .get(port_index as usize)
        .ok_or_else(|| "Invalid port index".to_string())?
        .clone();

    if !simulation_enabled() {
        let midi_in = MidiInput::new("nano").map_err(|e| e.to_string())?;
        let _ports = midi_in.ports();
        let _in_port = _ports
            .get(port_index as usize)
            .ok_or_else(|| "Port no longer available".to_string())?;

        *CONNECT_STATE.lock().unwrap() = Some(ConnectState {
            midi_in: Some(midi_in),
            port_index,
        });
    }

    Ok(port_name)
}

pub fn start_midi_event_stream(sender: EventSender) {
    if simulation_enabled() {
        start_simulated_stream(sender);
        return;
    }
    start_real_stream(sender);
}

fn start_real_stream(sender: EventSender) {
    let connect_state = CONNECT_STATE.lock().unwrap().take();
    let mut connect_state = match connect_state {
        Some(s) => s,
        None => {
            sender("ERROR: Not connected".to_string());
            return;
        }
    };

    let midi_in = match connect_state.midi_in.take() {
        Some(m) => m,
        None => {
            sender("ERROR: Already connecting".to_string());
            return;
        }
    };

    let ports = midi_in.ports();
    let in_port = match ports.into_iter().nth(connect_state.port_index as usize) {
        Some(p) => p,
        None => {
            sender("ERROR: Port no longer available".to_string());
            return;
        }
    };

    let callback_sender = sender.clone();
    let callback = move |_timestamp: u64, bytes: &[u8], _data: &mut ()| {
        if let Some(msg) = format_midi_event(bytes) {
            callback_sender(msg);
        }
    };

    match midi_in.connect(&in_port, "nano", callback, ()) {
        Ok(conn) => {
            *CONNECTION.lock().unwrap() = Some(conn);
            sender("CONNECTED".to_string());
        }
        Err(e) => {
            sender(format!("ERROR: {e}"));
        }
    }
}

fn start_simulated_stream(sender: EventSender) {
    let stop = Arc::new(AtomicBool::new(false));
    *SIM_STOP.lock().unwrap() = Some(stop.clone());

    let loops = if simulation_infinite() {
        u32::MAX
    } else {
        simulation_loop_count()
    };

    thread::Builder::new()
        .name("nano-midi-sim".into())
        .spawn(move || {
            sender("CONNECTED".to_string());

            for _ in 0..loops {
                if stop.load(Ordering::Relaxed) {
                    return;
                }
                for &note in SCALE_NOTES {
                    if stop.load(Ordering::Relaxed) {
                        return;
                    }
                    let msg_on = format_midi_event(&[0x90, note, 0x40]).unwrap_or_default();
                    sender(msg_on);
                    thread::sleep(Duration::from_millis(150));

                    if stop.load(Ordering::Relaxed) {
                        return;
                    }
                    let msg_off = format_midi_event(&[0x80, note, 0x00]).unwrap_or_default();
                    sender(msg_off);
                    thread::sleep(Duration::from_millis(30));
                }
            }
        })
        .ok();
}

pub fn disconnect_midi() {
    if let Some(stop) = SIM_STOP.lock().unwrap().take() {
        stop.store(true, Ordering::Relaxed);
    }
    *CONNECTION.lock().unwrap() = None;
}
