(* OCaml binding for Coherence *)

type coh_ptr  (* Pointer to a Cohml C++ object *)

(* Exception from Coherence itself *)
exception Coh_exception of (string * string) 
let _ = Callback.register_exception "Coh_exception" (Coh_exception ("Unknown", "Unknown Coherence exception"))

(* Exception from the Cohml binding *)
exception Cohml_exception of string
let _ = Callback.register_exception "Cohml_exception" (Cohml_exception ("Unknown"))

(* Connect/disconnect *)
external coh_getcache: string -> coh_ptr = "caml_coh_getcache"
external coh_shutdown: coh_ptr -> unit = "caml_coh_shutdown"

(* just strings for now *)
external coh_put: coh_ptr -> string -> string -> unit = "caml_coh_put"
external coh_get: coh_ptr -> string -> string = "caml_coh_get"
external coh_remove: coh_ptr -> string -> unit = "caml_coh_remove" (* would prefer del but this matches the underlying API *)

(* register callbacks for CohmlMapListener - insert, update, delete *)
external coh_listen: coh_ptr -> string -> string -> string ->unit = "caml_coh_addfilterlistener"
(* End of file *)
