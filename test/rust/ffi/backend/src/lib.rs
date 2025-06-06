#[no_mangle]
pub extern "C" fn add(left: u64, right: u64) -> u64 {
    left + right + 10
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}
