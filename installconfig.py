#! /usr/bin/env python3

import sys
import json

if __name__=="__main__":
    try:
        config=json.load(open("install_config.json"))
        debug=config["install_config_loader_settings"]["debug"]
    except Exception:
        sys.stderr.write("fail: cannot open install_config.json or load the settings for installconfig.py, is it corrupted?\n")
        sys.exit(1)
    except KeyboardInterrupt:
        sys.exit(127)
    for i in sys.argv[1:]:
        try:
            sys.stdout.write(config["manifest"][i])
        except KeyError:
            sys.stdout.write("")
            if(debug):
                raise
        except Exception:
            sys.stderr.write("fail: error found\n")
        except KeyboardInterrupt:
            sys.exit(127)
