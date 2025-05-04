#![allow(non_snake_case)]

mod baseline;
mod baseline_manipulated;
mod bomb;
mod candidate;
mod tile;
mod utils;
use std::io::Write;

use std::env;

fn main() {
    // env_logger::init();
    env_logger::Builder::new()
        .format(|buf, record| {
            writeln!(
                buf,
                "{} [{}] - {}",
                chrono::Local::now().format("%M:%S:%3f"),
                record.level(),
                record.args()
            )
        })
        .filter(None, log::LevelFilter::Trace)
        .init();

    let args: Vec<String> = env::args().collect();

    let algorithm = args[1].clone();
    let quiet: bool = args[2].contains("quiet");
    let n = args[3].parse::<usize>().unwrap();
    let b = args[4].parse::<usize>().unwrap();
    assert!(args.len() == 5 || args.len() == 6);
    let K = match args.len() {
        5 => 1,
        6 => args[5].parse::<usize>().unwrap(),
        _ => {
            log::error!("bad parameters");
            return;
        }
    };

    match algorithm.as_str() {
        "baseline" => baseline::main(n, b, quiet),
        "candidate" => candidate::main(n, b, quiet, K),
        "baseline_manipulated" => baseline_manipulated::main(n, b, quiet),
        &_ => {
            log::error!("unknown {}", algorithm);
        }
    }
    log::info!("done");
}
