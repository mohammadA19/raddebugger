
package core

// units

KB :: proc "contextless" (#any_int n: u64) -> u64 { return n << 10 }
MB :: proc "contextless" (#any_int n: u64) -> u64 { return n << 20 }
GB :: proc "contextless" (#any_int n: u64) -> u64 { return n << 30 }
TB :: proc "contextless" (#any_int n: u64) -> u64 { return n << 40 }

thousand :: proc "contextless" (#any_int n: u64) -> u64 { return n * 1000 }
million :: proc "contextless" (#any_int n: u64) -> u64 { return n * 1000000 }
billion :: proc "contextless" (#any_int n: u64) -> u64 { return n * 1000000000 }

