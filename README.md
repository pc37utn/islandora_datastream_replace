# Islandora Datastream Replace

## Introduction

This module provides a Drush script that can be used to bulk replace existing MODS or other datastreams, 
given a directory of files named (PID)_MODS.xml or (PID)_JP2.jp2, where the PID has the colon replaced with a underscore.

## Requirements

This module requires the following modules/libraries:

* [Islandora](https://github.com/islandora/islandora)
* [Tuque](https://github.com/islandora/tuque)

## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

## Troubleshooting/Issues

## Usage
Output of ```drush islandora_datastream_replace --help:```

```
Scans a source directory and replaces datastreams according to the given namespace and the filenames.

Examples:
 drush -u 1 islandora_datastream_replace --dsid=MODS --source=/tmp --namespace=islandora

Options:
 --namespace                               The namespace to match the pid named files with. Required.
 --dsid                                    The datastream id of the datastream. Required.
 --source                                  The directory to get the files of the datastreams from. Required.
  ```


The user option (-u) needs to be specified or errors could be
encountered when attempting to write the contents of the datastream to a file.

### How to Name Files

Islandora Datastream Replace expects files to be named with the namespace plus the pid plus the datastream id plus the extension all separated by underscopes.

For instance, if your namespace was scopes and your DSID was MODS, you filenames should look like this:

```
scopes_1_MODS.xml
scopes_10_MODS.xml
scopes_100_MODS.xml
scopes_11_MODS.xml
```

## Maintainers/Sponsors

Current maintainers:

* [pc37utn](https://github.com/pc37utn)

This project has been sponsored by:


## Development

If you would like to contribute to this module, please check out our helpful [Documentation for Developers](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers) info, as well as our [Developers](http://islandora.ca/developers) section on the Islandora.ca site.

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
