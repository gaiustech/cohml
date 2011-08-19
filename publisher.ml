(* generate random messages to test the subscribers *)

open Cohml
open Log_message
open Random
open Printf
open Unix
open Sys
open Array

type message = {msg_id : int;  msg_priority : int; msg_subject : string; msg_body : string}
external coh_put_message: coh_ptr -> message -> unit = "caml_put_message"
let print_message m = log_message (sprintf "id=%d, priority=%d, subject='%s', body='%s'" m.msg_id m.msg_priority m.msg_subject m.msg_body) 

let p () = (int 5) + 1 
let subjects = [|"INFO"; "WARN"; "FAIL"; "RECOVER"|] 
let s () = int (length subjects)
let bodies = [|"ABC"; "DEF"; "GHI"; "JKL"; "SOS"; "XYZ"|]
let b () = int (length bodies)

let () = 
  let x = (match length argv with |1 -> 10000|_ -> int_of_string argv.(1)) in
  let c = coh_getcache "message_cache" in
  let t1 = gettimeofday () in
  for i = 1 to x do
    coh_put_message c {msg_id = i; msg_priority = p (); msg_subject = subjects.(s ()); msg_body = bodies.(b ())}
  done;
  log_message (sprintf "Sent %d messages in %.3fs" x (gettimeofday () -. t1))

(* end of file *)
