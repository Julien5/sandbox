use flutter_rust_bridge::frb;

#[frb(sync)] // Synchronous mode for simplicity of the demo
pub fn greet(name: String) -> String {
    format!("Hello, {name}!")
}

#[frb(init)]
pub fn init_app() {
    // Default utilities - feel free to customize
    flutter_rust_bridge::setup_default_user_utils();
}

/* async */

#[cfg(not(target_family = "wasm"))]
use tokio::time::*;
#[cfg(target_family = "wasm")]
use wasmtimer::tokio::*;

pub async fn process(count: &i32) -> i32 {
    println!("start async sleep");
    sleep(std::time::Duration::from_millis(1000)).await;
    println!("end async sleep");
    count + 7
}

/* stream */

const SEC: std::time::Duration = std::time::Duration::from_millis(100);

use crate::frb_generated::StreamSink;
pub fn ticksink(sink: StreamSink<String>) -> anyhow::Result<()> {
    let mut ticks = 0;
    loop {
        let _ = sink.add(format!("ticks={}", ticks));
        let _ = std::thread::sleep(SEC);
        if ticks == i32::MAX {
            break;
        }
        ticks += 1;
        println!("rust {:2}", ticks);
    }
    Ok(())
}

#[frb(opaque)]
#[derive(Clone)]
pub struct EventSender {
    sink: StreamSink<String>,
}

pub trait Sender {
    fn send(&mut self, data: &String);
}

impl Sender for EventSender {
    fn send(&mut self, data: &String) {
        let _ = self.sink.add(data.clone());
    }
}

#[frb(opaque)]
pub struct Backend {
    pub sender: Option<EventSender>,
}

impl Backend {
    #[frb(sync)]
    pub fn make() -> Backend {
        Backend { sender: None }
    }
    #[frb(sync)]
    pub fn set_sink(&mut self, sink: StreamSink<String>) -> anyhow::Result<()> {
        self.sender = Some(EventSender { sink });
        Ok(())
    }
    fn send(&mut self, data: &String) {
        let _ = self.sender.as_mut().unwrap().send(&data);
    }

    pub async fn long_process(&mut self) {
        println!("long_process");
        for step in 0..100 {
            self.send(&format!("process: {}", step));
            let _ = wasmtimer::tokio::sleep(SEC).await;
            println!("rust:step: {}", step);
        }
    }
}
