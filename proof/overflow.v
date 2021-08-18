From Coq Require Import
  Arith
  Lia
  ZArith.

Open Scope Z.

Definition oversub (n m b : Z) : Z :=
  if m <? n then n - m else n + (b - m).

Notation "n -[ b ] m" := (oversub n m b) (at level 50, left associativity).

Lemma div_sub_distr_bound : forall m n q,
  0 <= m <= n -> 0 < q ->
  0 <= (n / q - m / q) - ((n - m) / q) <= 1.
Proof.
  intros * Hmn Hq.
  pose proof (Z.div_mod m q ltac:(lia)).
  pose proof (Z.div_mod n q ltac:(lia)).
  pose proof (Z.mod_pos_bound n q ltac:(lia)).
  pose proof (Z.mod_pos_bound m q ltac:(lia)).
  replace (n - m) with ((n mod q - m mod q) + (n / q - m / q) * q) by lia.
  rewrite Z.div_add by lia.
  enough (0 <= -((n mod q - m mod q) / q) <= 1) by lia.
  destruct (Z.le_gt_cases (m mod q) (n mod q)).
  { rewrite Z.div_small; lia. }
  destruct (Z.eq_dec ((n mod q - m mod q) mod q) 0).
  { rewrite <- Z.div_opp_l_z, Z.div_small; lia. }
  enough (-1 <= -((n mod q - m mod q) / q) - 1 <= 0) by lia.
  rewrite <- Z.div_opp_l_nz, Z.div_small; lia.
Qed.

Lemma div_diff_bound : forall m n p q,
  0 <= m <= n -> n - m < q * p -> 0 < q ->
  0 <= n / q - m / q <= p.
Proof.
  intros * Hmn Hq ?.
  pose proof (Z.div_le_mono m n q ltac:(lia) ltac:(lia)).
  split; [lia |].
  apply Z.div_lt_upper_bound in Hq as ?; [| lia].
  pose proof (div_sub_distr_bound m n q); lia.
Qed.

Lemma oversub_correct : forall m n b,
  0 <= m < n -> n - m < b ->
  (n mod b) -[b] (m mod b) = n - m.
Proof.
  unfold oversub; intros * Hmn Hb.
  rewrite !Z.mod_eq by lia.
  assert (n / b = m / b \/ n / b = m / b + 1) as [Heq | Heq]
    by (pose proof (div_diff_bound m n 1 b); lia).
  all: rewrite Heq; destruct (_ <? _) eqn:Hlt; rewrite ?Z.ltb_lt, ?Z.ltb_nlt in Hlt; lia.
Qed.
