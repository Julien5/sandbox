use std::fs::File; use std::io::{self, Read};

fn read_username_from_file() -> Result<String,io::Error> {
    let mut username = String::new();

    File::open("hello.txt")?.read_to_string(&mut username)?;

    Ok(username)
}

fn read_username_wrapper() -> Result<u32, std::io::Error> {
	let foo=read_username_from_file()?;
	Ok(foo.chars().count().try_into().unwrap())
}

// This function takes ownership of the heap allocated memory
fn destroy_box(c: Box<i32>) {
    println!("Destroying a box that contains {}", c);

    // `c` is destroyed and the memory freed
}

fn main() {
	let _=read_username_wrapper();
    let a=String::from("hello");
    let b=a.clone();
    println!("a is {}, and b is {}", a,b);
    // _Stack_ allocated integer

    // *Copy* `x` into `y` - no resources are moved
	let x = 1;
    let y = x;

    // Both values can be independently used
    println!("x is {}, and y is {}", x, y);

    // `a` is a pointer to a _heap_ allocated integer
    let a = Box::new(5i32);

    println!("a contains: {}", a);

    // *Move* `a` into `b`
    let b = a.clone();
    // The pointer address of `a` is copied (not the data) into `b`.
    // Both are now pointers to the same heap allocated data, but
    // `b` now owns it.
    
    // Error! `a` can no longer access the data, because it no longer owns the
    // heap memory
    println!("a contains: {}", a);
    // TODO ^ Try uncommenting this line

    // This function takes ownership of the heap allocated memory from `b`
    destroy_box(b);

    // Since the heap memory has been freed at this point, this action would
    // result in dereferencing freed memory, but it's forbidden by the compiler
    // Error! Same reason as the previous Error
    //println!("b contains: {}", b);
    // TODO ^ Try uncommenting this line
}


#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        let result = 2+2;
        assert_eq!(result, 4);
    }
}
