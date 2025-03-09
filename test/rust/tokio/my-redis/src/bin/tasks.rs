use tokio::time::Duration;

struct Foo {
	n: i32,
}

impl Foo {
	fn new(n:i32) -> Foo {
		Foo{n:n}
	}
	async fn compute(&mut self) {
		println!("Foo-{} compute starts",self.n);
		tokio::time::sleep(Duration::from_secs(1)).await;
		self.n=self.n+1;
		println!("Foo-{} compute done",self.n);
	}
}

#[tokio::main]
async fn main() {
	let mut foo1 = Foo::new(0);
    let task1 = tokio::spawn(async move {
		foo1.compute().await;
    });
	let mut foo2 = Foo::new(2);
	let task2 = tokio::spawn(async move {
        foo2.compute().await;
    });
	task1.await.unwrap();
	task2.await.unwrap();
}
