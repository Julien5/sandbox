use tokio::time::Duration;

struct Loader {
	url: String,
}

struct Sink {
	n: i32,
}

impl Sink {
	fn new(n:i32) -> Sink {
		Sink{n:n}
	}
	fn print(self) {
		println!("sink-{}",self.n);
	}
}

async fn wait(n:i32) {
	tokio::time::sleep(Duration::from_secs(n as u64)).await;
}

impl Loader {
	fn new(url:String) -> Loader {
		Loader{url:url}
	}
	async fn run(&mut self) -> Sink {
		println!("Loader-{} step 1",self.url);
		wait(1).await;
		println!("Loader-{} step 2",self.url);
		wait(1).await;
		println!("Loader-{} step 3",self.url);
		wait(1).await;
		let s = Sink::new(1);
		s
	}
}

fn start_loader(s:String) -> tokio::task::JoinHandle<Sink> {
	let mut foo = Loader::new(s.clone());
	let handle = tokio::spawn(async move {
		foo.run().await
	});
	handle
}


#[tokio::main]
async fn main() {
    let task1 = start_loader("fip".to_string());
	let task2 = start_loader("france-musique".to_string());
	wait(2).await;
	task2.abort();
	let res=task1.await.unwrap();
	res.print();
	//println!("task1 is finished {}",task1.is_finished());
	//println!("task2 is finished {}",task2.is_finished());
}
