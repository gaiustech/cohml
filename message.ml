(* message example *)

open Cohml
open Printf

type message = {msg_id : int;  msg_priority : int; msg_subject : string; msg_body : string}

external coh_put_message: coh_ptr -> message -> unit = "caml_put_message"
external coh_get_message: coh_ptr -> int -> message = "caml_get_message"

let print_message m = printf "id=%d, priority=%d, subject='%s', body='%s'\n" m.msg_id m.msg_priority m.msg_subject m.msg_body

let () = 
  let test_msg = {msg_id = 1; msg_priority=3; msg_subject="test"; msg_body="hello, world!"} in
  let c = coh_getcache "message_cache" in
  coh_put_message c test_msg;
  print_message (coh_get_message c 2);
  
(* end of file *)
