(* generate random messages to test the subscribers *)

open Cohml
open Log_message

type message = {msg_id : int;  msg_priority : int; msg_subject : string; msg_body : string}
external coh_put_message: coh_ptr -> message -> unit = "caml_put_message"
let print_message m = log_message (Printf.sprintf "id=%d, priority=%d, subject='%s', body='%s'" m.msg_id m.msg_priority m.msg_subject m.msg_body) 

let p () = (Random.int 5) + 1 
let subjects = [|"INFO"; "WARN"; "FAIL"; "RECOVER"|] 
let s () = Random.int (Array.length subjects)
let bodies = [|"ABC"; "DEF"; "GHI"; "JKL"; "SOS"; "XYZ"|]
let b () = Random.int (Array.length bodies)
let pad = String.make 40 'Z' (* for Tibco-style 50-byte messages *) 

let () = 
  Random.self_init ();
  let x = (match Array.length Sys.argv with |1 -> 10000|_ -> int_of_string Sys.argv.(1)) in
  let c = coh_getcache "message_cache" in
  let t1 = Unix.gettimeofday () in
  for i = 1 to x do
    let m = {msg_id = i; msg_priority = p (); msg_subject = subjects.(s ()); msg_body = pad ^ (bodies.(b ()))} in
    coh_put_message c m;
    print_message m;
    Unix.sleep 1
  done;
  log_message (Printf.sprintf "Sent %d messages in %.3fs" x (Unix.gettimeofday () -. t1))

(* end of file *)
