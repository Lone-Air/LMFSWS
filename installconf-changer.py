#! /usr/bin/env python3

import sys
import json

if __name__=="__main__":
    try:
        config=json.load(open("install_config.json"))
    except Exception:
        sys.stderr.write("fail: cannot open install_config.json or load, is it corrupted?\n")
        sys.exit(1)
    except KeyboardInterrupt:
        sys.exit(127)
    try:
        for i in sys.argv[1:]:
            try:
                _args=i.split("=")
                _name=_args[0]
                _head=_name.split(".")[0]
                _item=_name.split(".")[1]
                _arg='='.join(_args[1:])
                if(_arg=="true"): _arg=True
                elif(_arg=="false"): _arg=False
                config[_head][_item]=_arg
            except Exception:
                sys.stderr.write("fail: error found\n")
                raise
            except KeyboardInterrupt:
                sys.exit(127)
    except KeyboardInterrupt:
        sys.exit(127) 
    try:
        json.dump(config, open("install_config.json", "w"), indent=4, ensure_ascii=False)
    except Exception:
        sys.stderr.write("fail: failed to save\n")
    except KeyboardInterrupt:
        sys.exit(127)
