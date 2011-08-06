(* Coherence MapListener example *)
open Cohml
open Callback
open Unix

let cbf_coh_insert k v = print_endline ("Inserted key=" ^ k ^ " value=" ^ v)
let _ = register "cbf_coh_insert" cbf_coh_insert

let cbf_coh_update k ov nv = print_endline ("Updated key=" ^ k ^ " old value=" ^ ov ^ " new value=" ^ nv)
let _ = register "cbf_coh_update" cbf_coh_update

let cbf_coh_delete k = print_endline ("Deleted key=" ^ k)
let _ = register "cbf_coh_delete" cbf_coh_delete

let () = 
  print_endline "Listening...";
  let c = coh_getcache "test_cache" in
  coh_listen c ;
  sleep 1000

(* End of file *)
