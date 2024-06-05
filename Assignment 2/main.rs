use std::{
    fs,
    io::{prelude::*, BufReader},
    net::{TcpListener, TcpStream},
};

fn main() {
    let listener = TcpListener::bind("127.0.0.1:7878").unwrap();
    for connection in listener.incoming(){
        let connection = connection.unwrap();
        // println!("Person here!");
        handle_connection(connection);
    }
}

fn send_response(mut code : u32, file : &str, mut stream : &TcpStream) -> (){ 
    let contents = match fs::read_to_string(file) {
        Ok(file) => file,
        Err(_) => {code = 500;fs::read_to_string("500.html").unwrap()}, 
    };
    let length = contents.len();

    if code == 200 {
        let status_line = "HTTP/1.1 200 OK";
        
        let response = format!("{status_line}\r\nContent-Length : {length}\r\n\r\n{contents}");
        stream.write_all(response.as_bytes()).unwrap();
    }else if code == 404{
        let status_line = "HTTP/1.1 404 Not Found";
        let response = format!("{status_line}\r\nContent-Length : {length}\r\n\r\n{contents}");
        stream.write_all(response.as_bytes()).unwrap();
    }else if code == 400{
        let status_line = "HTTP/1.1 400 Bad Request";
        let response = format!("{status_line}\r\nContent-Length : {length}\r\n\r\n{contents}");
        stream.write_all(response.as_bytes()).unwrap();
    }else if code == 500{
        let status_line = "HTTP/1.1 500 Internal Server Error";
        let response = format!("{status_line}\r\nContent-Length : {length}\r\n\r\n{contents}");     
        stream.write_all(response.as_bytes()).unwrap();

    }else{
        stream.write_all("  ".to_string().as_bytes()).unwrap();
    }
    
}

fn handle_connection(mut stream : TcpStream){
    let buf_reader = BufReader::new(&mut stream);
    let buf_reader = buf_reader.lines().next().unwrap().unwrap();
    let mut request = buf_reader.split_whitespace();
    let type_request = request.next().unwrap();
    let resource = request.next().unwrap();
    let http_version = request.next().unwrap();
    
    if resource == "/" && type_request == "GET" && http_version == "HTTP/1.1" {
        send_response(200,"main.html",&stream);
        // stream.write_all(response.as_bytes()).unwrap();
    }else if type_request != "GET" || http_version != "HTTP/1.1" {
        send_response(400,"400.html",&stream);
        // stream.write_all(response.as_bytes()).unwrap();
    }else{
        send_response(404,"404.html",&stream);
        // stream.write_all(response.as_bytes()).unwrap();
    }
    
    

    
}