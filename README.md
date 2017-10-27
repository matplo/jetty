# jetty
Jets with pythia &amp; fastjet &amp; root sandbox

# requirements

 - [CMAKE](https://cmake.org)
 - [ROOT](http://root.cern.ch) - `root-config` should be in the $PATH
 - [Pythia8](http://home.thep.lu.se/~torbjorn/Pythia.html) - `pythia8-config` should be in the $PATH.
 - [BOOST](http://www.boost.org)
 
# provided simple - installation

 - uses GIT
 - installs to `<dir_where_downloaded>/jetty_1.0`
 - makes a module file (see [module package](http://modules.sourceforge.net)) in `<dir_where_downloaded>/modules` - depends on modules as in [HEPSOFT](https://github.com/matplo/hepsoft)

```bash
$ <dir_where_downloaded>/scripts/build_jetty.sh --build --module
```

# run an example

```bash
$ module use <where_hepsoft_modules>
$ module load hepsoft/pythia8
$ module use <dir_where_downloaded>/modules
$ module load jetty/1.0
$ jettyExamplesExe --pythia
```
