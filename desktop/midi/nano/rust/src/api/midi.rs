use crate::frb_generated::StreamSink;
use crate::midi_core;
use std::sync::Arc;

#[flutter_rust_bridge::frb(sync)]
pub fn list_midi_ports() -> Vec<String> {
    midi_core::list_midi_ports()
}

pub fn connect_midi(port_index: u32) -> Result<String, String> {
    midi_core::connect_midi(port_index)
}

pub fn start_midi_event_stream(sink: StreamSink<String>) {
    let sender: midi_core::EventSender =
        Arc::new(move |msg| drop(sink.add(msg)));
    midi_core::start_midi_event_stream(sender);
}

pub fn disconnect_midi() {
    midi_core::disconnect_midi();
}
