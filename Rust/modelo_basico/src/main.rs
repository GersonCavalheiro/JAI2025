use std::thread;

fn main() {
    let x: i32 = 8;
    let r: i32;
    let t = thread::spawn(move || { x * x });
    r = t.join().unwrap();
    print!("{}\n", r);
}
