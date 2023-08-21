# Tlaxcaltin

_Tlaxcaltin_ (plural of _tlaxcalli_, which is Nahuatl for _tortilla_) is a collection of wraps and corresponding package files for use in other projects.
Sadly, the wrap system does not seem to work with nested git submodules or subtrees, which is why I use the git `pre-commit` hook to keep the wraps updated instead:

```sh
#!/bin/sh
if [ ! -d "./subprojects/" ]; then
  git clone git@github.com:Fingolfin1196/tlaxcaltin.git subprojects
fi
python3 ./subprojects/update_tlaxcaltin.py
```

This has the unfortunate downside of having to download the dependencies anew each time a commit is performed, but it’s the best I’ve got.
