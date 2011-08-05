(* OCaml binding for Coherence *)

type coh_ptr  (* Pointer to a Cohml C++ object *)

exception Coh_exception of (string * string) 
let _ = Callback.register_exception "Coh_exception" (Coh_exception ("Unknown", "Unknown Coherence exception"))

(* Connect/disconnect *)
external coh_getcache: string -> coh_ptr = "caml_coh_getcache"
(*external coh_shutdown: unit -> unit = "caml_coh_shutdown"

(* just strings for now *)
external coh_put: coh_ptr -> string -> string -> unit = "caml_coh_put"
external coh_get: coh_ptr -> string -> string = "caml_coh_get"*)

(* End of file *)
