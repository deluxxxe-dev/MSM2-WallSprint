# v0.1c1 actual result

The game accepted Spider-Man2.exe 2.810.0.0 and its expected SHA256. The c1 proof nevertheless failed with writer_verified=1, caller_verified=0, decoded_prefix_bytes=5 and pairs=0. Both source override counts stayed zero, explaining the reported absence of visible rotation.

This was reproduced using the original 444-byte writer and 128-byte caller supplied privately in the current task. Their SHA256 values are respectively `ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196` and `1665a7a329a983101673f05ccd95bbf3d738c39099469b4acfd14344ec98a79d`.

The exact published c1 decoder (implementation commit `71437fac6c0640af01725a46e1b49c5627c739bb`) reproduces writer admission 0, decoded bytes 5, pairs 0 and caller admission 0. The writer's first instruction is a normal save of RBX into the caller stack. c1 treated an unrelated memory base as a fatal missing RDX/RCX fact. Its HDE backend also does not support the following VEX instructions or the caller's VEX scalar products.

The c1 source ownership/retention logic was therefore not reached by this actual run. It is retained for c2; no c1 game retention PASS is claimed.
