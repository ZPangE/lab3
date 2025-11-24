use std::io::{self, Write};

fn read_line() -> String {
    let mut input = String::new();
    io::stdin().read_line(&mut input).expect("입력 오류");
    input.trim().to_string()
}

fn read_usize_pair() -> (usize, usize) {
    loop {
        let line = read_line();
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.len() == 2 {
            if let (Ok(r), Ok(c)) = (parts[0].parse::<usize>(), parts[1].parse::<usize>()) {
                return (r, c);
            }
        }
        println!("형식: <행 개수> <열 개수>  예) 2 3");
        print!("다시 입력: ");
        io::stdout().flush().unwrap();
    }
}

fn read_matrix(rows: usize, cols: usize, name: &str) -> Vec<Vec<i32>> {
    println!("행렬 {}의 원소를 입력하세요 (총 {}행, 각 행마다 {}개):", name, rows, cols);

    let mut mat = Vec::with_capacity(rows);

    for i in 0..rows {
        loop {
            print!("{}행 입력: ", i + 1);
            io::stdout().flush().unwrap();
            let line = read_line();
            let nums: Vec<&str> = line.split_whitespace().collect();

            if nums.len() != cols {
                println!("오류: 정확히 {}개 입력해야 합니다.", cols);
                continue;
            }

            let mut row_vec = Vec::with_capacity(cols);
            let mut ok = true;

            for token in nums {
                match token.parse::<i32>() {
                    Ok(v) => row_vec.push(v),
                    Err(_) => {
                        println!("정수만 입력하세요.");
                        ok = false;
                        break;
                    }
                }
            }

            if ok {
                mat.push(row_vec);
                break;
            }
        }
    }

    mat
}

fn add_matrices(a: &Vec<Vec<i32>>, b: &Vec<Vec<i32>>) -> Vec<Vec<i32>> {
    let rows = a.len();
    let cols = if rows > 0 { a[0].len() } else { 0 };

    let mut result = Vec::with_capacity(rows);

    for i in 0..rows {
        let mut row = Vec::with_capacity(cols);
        for j in 0..cols {
            row.push(a[i][j] + b[i][j]);
        }
        result.push(row);
    }

    result
}

fn print_matrix(mat: &Vec<Vec<i32>>, name: &str) {
    println!("{}:", name);
    for row in mat {
        for val in row {
            print!("{} ", val);
        }
        println!();
    }
}

fn main() {
    println!("=== 행렬 덧셈 프로그램 (Rust / Vec) ===");

    print!("행 개수와 열 개수를 입력하세요 (예: 2 3): ");
    io::stdout().flush().unwrap();
    let (rows, cols) = read_usize_pair();

    if rows == 0 || cols == 0 {
        println!("행과 열은 0이 될 수 없습니다.");
        return;
    }

    let a = read_matrix(rows, cols, "A");
    let b = read_matrix(rows, cols, "B");

    if a.len() != b.len() || a[0].len() != b[0].len() {
        println!("오류: 두 행렬의 크기가 다릅니다.");
        return;
    }

    let c = add_matrices(&a, &b);

    println!("\n--- 결과 ---");
    print_matrix(&a, "행렬 A");
    print_matrix(&b, "행렬 B");
    print_matrix(&c, "A + B");
}
