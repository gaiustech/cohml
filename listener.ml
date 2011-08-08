(* Coherence MapListener example *)
open Cohml
open Callback
open Unix

let _ = register "cbf_coh_insert" (fun k v -> print_endline ("Inserted key=" ^ k ^ " value=" ^ v))
let _ = register "cbf_coh_update" (fun k ov nv -> print_endline ("Updated key=" ^ k ^ " old value=" ^ ov ^ " new value=" ^ nv))
let _ = register "cbf_coh_delete" (fun k -> print_endline ("Deleted key=" ^ k))

let () = 
  print_endline "Listening...";
  let c = coh_getcache "test_cache" in
  coh_listen c "cbf_coh_insert" "cbf_coh_update" "cbf_coh_delete";
  sleep 1000

(* End of file *)
