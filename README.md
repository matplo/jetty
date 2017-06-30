# jetty
Jets with pythia &amp; fastjet &amp; root sandbox

# requirements

 - [ROOT](http://root.cern.ch) - `root-config` should be in the $PATH
 - [Pythia8](http://home.thep.lu.se/~torbjorn/Pythia.html) - `pythia8-config` should be in the $PATH.
 - [BOOST](http://www.boost.org)

# installation

```bash
$ <dir_where_downloaded>/scripts/make_module.sh -mc
$ module load use.own
$ module load jetty/default
$ build_jetty.sh
```

# example

$  jettyExamplesExe --pythia

