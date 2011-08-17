(* message example *)

open Cohml
open Printf
open Log_message

type message = {msg_id : int;  msg_priority : int; msg_subject : string; msg_body : string}

external coh_put_message: coh_ptr -> message -> unit = "caml_put_message"
external coh_get_message: coh_ptr -> int -> message = "caml_get_message"
external coh_pri_message: coh_ptr -> int -> message list = "caml_query_message_pri"

let print_message m = log_message (sprintf "id=%d, priority=%d, subject='%s', body='%s'\n" m.msg_id m.msg_priority m.msg_subject m.msg_body)

let () = 
  let test_msgs = [{msg_id = 1; msg_priority=3; msg_subject="test";    msg_body="hello, world!"};
		   {msg_id = 2; msg_priority=1; msg_subject="urgent!"; msg_body="High priority message"};
		   {msg_id = 3; msg_priority=1; msg_subject="urgent!"; msg_body="High priority message 2"}] in
  let c = coh_getcache "message_cache" in
  List.iter (coh_put_message c) test_msgs;
  List.iter print_message (coh_pri_message c 2)
  
(* end of file *)
