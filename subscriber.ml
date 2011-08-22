(* pubsub subscriber *)

open Cohml
open Callback
open Printf
open Log_message
open Unix

(* the message record struct *)
type message = {msg_id : int;  msg_priority : int; msg_subject : string; msg_body : string}

(* the query predicate for the message records we are interested in receiving 
   events for. Fields are 0-3 (see definition of message above), conditions are
   0: less than, 1: equal to, 2: greater than, 3: like, types are 0 for int, 1
   for string. Search term is a string and we'll atoi() it in C if we need to.
   Existing is if we want anything already in the cache before we begin.
*)
type field = | ID | PRIORITY | SUBJECT | BODY
type field_type = | INT | STRING
type condition = | LESS_THAN | EQUAL_TO | GREATER_THAN | LIKE 
type query = {field : field; field_type: field_type; condition : condition; search_term : string; existing: bool}

(* function to register message listener callbacks - insert, update, delete*)
external coh_msg_listen: coh_ptr -> query -> string -> string -> string -> unit = "caml_coh_addmessagelistener"

let string_of_message m = sprintf "id=%d, priority=%d, subject='%s', body='%s'" m.msg_id m.msg_priority m.msg_subject m.msg_body

(* callback functions *)
let _ = register "cbf_msg_insert" (fun k m -> 
  log_message (sprintf "New message: %s" (string_of_message m)))
let _ = register "cbf_msg_update" (fun k om nm ->
  log_message (sprintf "Message changed %s/%s" (string_of_message om) (string_of_message nm)))
let _ = register "cbf_msg_delete" (fun k ->
  log_message (sprintf "Message %d deleted" k))
  
let () =
  let st = (match Array.length Sys.argv with
    |1 -> 1 (* default to subscriber type 1 *)
    |_ -> int_of_string Sys.argv.(1) ) in
  let q = (match st with
    |1 -> { field=PRIORITY; field_type=INT; condition=LESS_THAN; search_term="2"; existing=false }
    |2 -> { field=SUBJECT; field_type=STRING; condition=EQUAL_TO; search_term="INFO"; existing=false }
    |_ -> { field=BODY; field_type=STRING; condition=LIKE; search_term="%XYZ%"; existing=false }) in
  log_message (sprintf "Subscriber starting as type %d" st);
  let c = coh_getcache "message_cache" in
  coh_msg_listen c q "cbf_msg_insert" "cbf_msg_update" "cbf_msg_delete";
  sleep 1000

(* end of file *)
