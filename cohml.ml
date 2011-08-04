(* OCaml binding for Coherence *)

type coh_ptr  (* Pointer to a Cohml C++ object *)

external coh_getcache: unit -> coh_ptr = "caml_coh_getcache"

(* just strings for now *)
external coh_put: coh_ptr -> string -> string -> unit = "caml_coh_put"
external coh_get: coh_ptr -> string -> string -> "caml_coh_get"

(* End of file *)
