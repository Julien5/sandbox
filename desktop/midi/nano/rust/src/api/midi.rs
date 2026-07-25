use crate::frb_generated::StreamSink;
use midir::{MidiInput, MidiInputConnection};
use std::sync::Mutex;

const NOTE_NAMES: &[&str] = &[
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B",
];

struct ConnectState {
    midi_in: Option<MidiInput>,
    port_index: u32,
}

static CONNECT_STATE: Mutex<Option<ConnectState>> = Mutex::new(None);
static CONNECTION: Mutex<Option<MidiInputConnection<()>>> = Mutex::new(None);

#[flutter_rust_bridge::frb(sync)]
pub fn list_midi_ports() -> Vec<String> {
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
    let midi_in = MidiInput::new("nano").map_err(|e| e.to_string())?;
    let ports = midi_in.ports();
    let in_port = ports
        .get(port_index as usize)
        .ok_or_else(|| "Invalid port index".to_string())?;
    let port_name = midi_in.port_name(in_port).map_err(|e| e.to_string())?;

    *CONNECT_STATE.lock().unwrap() = Some(ConnectState {
        midi_in: Some(midi_in),
        port_index,
    });

    Ok(port_name)
}

pub fn start_midi_event_stream(sink: StreamSink<String>) {
    let connect_state = CONNECT_STATE.lock().unwrap().take();
    let mut connect_state = match connect_state {
        Some(s) => s,
        None => {
            let _ = sink.add("ERROR: Not connected".to_string());
            return;
        }
    };

    let midi_in = match connect_state.midi_in.take() {
        Some(m) => m,
        None => {
            let _ = sink.add("ERROR: Already connecting".to_string());
            return;
        }
    };

    let ports = midi_in.ports();
    let in_port = match ports.into_iter().nth(connect_state.port_index as usize) {
        Some(p) => p,
        None => {
            let _ = sink.add("ERROR: Port no longer available".to_string());
            return;
        }
    };

    let callback_sink = sink.clone();
    let callback = move |_timestamp: u64, bytes: &[u8], _data: &mut ()| {
        if bytes.len() < 3 {
            return;
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

        let msg = format!("{event_type} {raw} {name}");
        let _ = callback_sink.add(msg);
    };

    match midi_in.connect(&in_port, "nano", callback, ()) {
        Ok(conn) => {
            *CONNECTION.lock().unwrap() = Some(conn);
            sink.add("CONNECTED".to_string()).ok();
        }
        Err(e) => {
            sink.add(format!("ERROR: {e}")).ok();
        }
    }
}

pub fn disconnect_midi() {
    *CONNECTION.lock().unwrap() = None;
}
