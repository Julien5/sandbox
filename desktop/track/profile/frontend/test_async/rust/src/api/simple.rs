#[flutter_rust_bridge::frb(sync)] // Synchronous mode for simplicity of the demo
pub fn greet(name: String) -> String {
    format!("Hello, {name}!")
}

#[flutter_rust_bridge::frb(init)]
pub fn init_app() {
    // Default utilities - feel free to customize
    flutter_rust_bridge::setup_default_user_utils();
}

const ONE_SECOND: std::time::Duration = std::time::Duration::from_millis(25);

use crate::frb_generated::StreamSink;
pub fn ticksink(sink: StreamSink<String>) -> anyhow::Result<()> {
    let mut ticks = 0;
    loop {
        let _ = sink.add(format!("ticks={}", ticks));
        let _ = std::thread::sleep(ONE_SECOND);
        if ticks == i32::MAX {
            break;
        }
        ticks += 1;
        println!("rust {:2}", ticks);
    }
    Ok(())
}

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
