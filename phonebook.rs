use std::collections::HashMap;
use std::io::{self, Write};

fn read_line() -> String {
    let mut s = String::new();
    io::stdin().read_line(&mut s).expect("입력 오류");
    s.trim().to_string()
}

fn main() {
    let mut phonebook: HashMap<String, String> = HashMap::new();

    loop {
        println!("\n=== 전화번호부 메뉴 ===");
        println!("1. 추가");
        println!("2. 검색");
        println!("3. 전체 목록 보기");
        println!("4. 종료");
        print!("메뉴 번호 선택: ");
        io::stdout().flush().unwrap();

        let choice = read_line();

        match choice.as_str() {
            "1" => {
                print!("이름 입력: ");
                io::stdout().flush().unwrap();
                let name = read_line();

                print!("전화번호 입력: ");
                io::stdout().flush().unwrap();
                let number = read_line();

                phonebook.insert(name.clone(), number.clone());
                println!("✔ {} 님의 번호({})가 저장되었습니다.", name, number);
            }

            "2" => {
                print!("검색할 이름 입력: ");
                io::stdout().flush().unwrap();
                let name = read_line();

                match phonebook.get(&name) {
                    Some(num) => println!("📞 {} 님의 번호: {}", name, num),
                    None => println!("❌ '{}' 님은 등록되어 있지 않습니다.", name),
                }
            }

            "3" => {
                println!("\n=== 전체 전화번호부 ===");
                if phonebook.is_empty() {
                    println!("(비어 있음)");
                } else {
                    for (name, number) in &phonebook {
                        println!("{} : {}", name, number);
                    }
                }
            }

            "4" => {
                println!("프로그램을 종료합니다.");
                break;
            }

            _ => println!("❗ 잘못된 선택입니다. 1~4 중에서 입력하세요."),
        }
    }
}
