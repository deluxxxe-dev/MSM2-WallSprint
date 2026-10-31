"""Regression for root-token inventory churn; owned /proc models only, not WSL proof."""
import io
import os
import types
import unittest
from unittest.mock import patch

import operation_lock as o


def stat_text(pid,start):
    return f"{pid} (owned) S " + " ".join(["0"]*18+[str(start)])


class InventoryChurnTests(unittest.TestCase):
    def probe(self,listdir,stat_start):
        ns={"__name__":"owned_inventory_probe"}
        exec(o.WSL_PROBE,ns)

        def opened(path,mode="r"):
            pid=int(path.split("/")[2])
            name=path.rsplit("/",1)[-1]
            if name=="stat":
                return io.StringIO(stat_text(pid,stat_start(pid)))
            if name=="environ":
                return io.BytesIO(b"OTHER=owned\0")
            raise AssertionError((path,mode))

        with patch.dict(os.environ,{"WSL_DISTRO_NAME":"Owned"}), \
             patch("os.getuid",return_value=0), \
             patch("os.geteuid",return_value=0), \
             patch("os.getpid",return_value=999), \
             patch("os.listdir",side_effect=listdir), \
             patch("os.stat",return_value=types.SimpleNamespace(st_uid=0)), \
             patch("builtins.open",side_effect=opened), \
             patch("os.pidfd_open",return_value=42), \
             patch("os.close"):
            return ns["probe"]("token",False,"Owned",0,True)

    def test_departure_only_inventory_churn_is_quiet(self):
        calls=[0]
        def ls(path):
            self.assertEqual(path,"/proc")
            calls[0]+=1
            return ["401"] if calls[0]%2 else []
        result=self.probe(ls,lambda pid:1)
        self.assertEqual(result["result"],"QUIET")
        self.assertNotIn("inventory_unstable",result["counts"])
        self.assertEqual(calls[0],2)

    def test_new_identity_each_final_inventory_remains_unknown(self):
        calls=[0]
        def ls(path):
            self.assertEqual(path,"/proc")
            calls[0]+=1
            return [str(400+calls[0])]
        result=self.probe(ls,lambda pid:pid)
        self.assertEqual(result["result"],"UNKNOWN")
        self.assertEqual(result["counts"].get("inventory_unstable"),1)
        event=next(e for e in result["events"] if e["category"]=="inventory_unstable")
        self.assertEqual(event["new_or_reused"],1)
        self.assertEqual(calls[0],6)


if __name__=="__main__":
    unittest.main()
