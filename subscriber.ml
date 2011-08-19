(* pubsub subscriber *)

open Cohml
open Callback
open Printf
open Log_message
open Unix

(* function to register message listener callbacks - insert, update, delete*)
external coh_msg_listen: coh_ptr -> string -> string -> string -> unit = "caml_coh_addmessagelistener"

type message = {msg_id : int;  msg_priority : int; msg_subject : string; msg_body : string}

let string_of_message m = sprintf "id=%d, priority=%d, subject='%s', body='%s'" m.msg_id m.msg_priority m.msg_subject m.msg_body

(* callback functions *)
let _ = register "cbf_msg_insert" (fun k m -> 
  log_message (sprintf "New message: %s" (string_of_message m)))
let _ = register "cbf_msg_update" (fun k om nm ->
  log_message (sprintf "Message changed %s/%s" (string_of_message om) (string_of_message nm)))
let _ = register "cbf_msg_delete" (fun k ->
  log_message (sprintf "Message %d deleted" k))

(* first version - just listening *)
let () =
  log_message "Listening for ALL messages";
  let c = coh_getcache "message_cache" in
  coh_msg_listen c "cbf_msg_insert" "cbf_msg_update" "cbf_msg_delete";
  sleep 1000

(* end of file *)
