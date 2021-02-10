# Machine Learning Static Tensor in C 	(MLST-C, can be MLST-py)

ML Kernel is written in C.
Learning Programs & Server are written in C (converted to linux-exe).
Data & Model managment & generation are mainly written in Python.

## Principles

A **Tensor** is an any dimentions tensor (deep). Values are stored in one-dimention array of ```double```. Naviate throught dimentions and shapes is made with ```realpos(tensor, position)```. Tensor shapes order is from smallest to biggest dimention (it's intuitive like x,y,z..).

A **Model** is a C-struct ```(definded in include/model.h:Session)``` whitch has a pointer to an Instructions Set (definded in include/model.h:Session) and a to an Tensors Set (Variables,and its first order gradient).

A **Session** has only one Instruction Set for many Models.

A **MetaSession** is an array of sessions (in result many Instructions set). Can be start as Server (even with only 1 Session with one Model and one code)

## Commands

All commands start from ```size_t``` command id, then goes all params (look at Commands file) of ```size_t``` type.