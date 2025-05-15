#   Developer Reference Overview

Version: `1.0.0dr` | Handbook Last Updated: May 14, 2025

This first volume of the *ByteFrost Assembler Handbook* documents the ByteFrost
Assembler software architecture, including the software pipeline, each of its
stages, and important definitions / algorithm descriptions. It is written for
the intended use of a maintainer or expander of the ByteFrost Assembler
capabilities, and so contains sections for how to expand the ByteFrost Assembler
in each pipeline stage.

Changes to the ByteFrost Assembler's architecture (i.e., updates that change the
number of stages, the data they pass between them, etc.) are noted in a later
section. These are distinguished from "content changes," i.e., expansions or 
modifications of existing functionality (such as adding new preprocessor 
directives or assembly instructions) that already exist in the current design of
the Assembler.

##  Developer Reference Changelog

### Version `1.0.0dr`

*   The initial version of the Developer Reference handbook.

##  Assembler Design Changelog

### Version `1.0.0a`

*   This is the original version of the ByteFrost Assembler design, with 5 
software pipeline stages (CLAP, Parser, Preprocessor, Label Handler, and Code
Generation).

##  Note on Semantic Versioning

**TODO: Rewrite this section on semantic versioning to be more precise**

There are three relevant semantic version number schemes at play:

1.  `x.y.zdr` - this is the semantic version number of the Developer Reference
    volume.
2.  `x.y.zpm` - this is the semantic version number of the Programmer's Manual
    volume.
3.  `x.y.za` - this is the semantic version number of the ByteFrost Assembler.

Semantic version numbers are incremented in one way for the handbook volumes and
in another for the ByteFrost Assembler software:
### Semantic Versioning of Handbook Volumes
*   Let the semantic version number of a handbook be `x.y.z` for the present
    version $v$. A new change $C$ is applied to the handbook so that the
    new version is $v'$. The version number of $v'$ can be any of the
    following: `(x + 1).0.0`, `x.(y + 1).0`, or `x.y.(z + 1)`.
1.  If $C$ constitutes a minor change to a subset of sections in the
    handbook, and now section is added or removed, then the `z` component of
    the version number should be incremented (The new version number should
    be `x.y.(z + 1)`).
    *   A minor change does not fundamentally alter the essence of the
        affected sections; otherwise, it could be construed as the removal
        of the original section and the introduction of a new one.
2.  If $C$ introduces a new section, removes a section, or alters a section
    fundamentally as to change its essence / structure, then the `y`
    component of the version number should be incremented (The new version
    number should be `x.(y + 1).0`).
3.  If $C$ changes the structure of the handbook at a large scale or a large
    amount of written material is added or removed (i.e., more than just a
    few sections are added, modified, or removed) as to be incompatible with
    the previous version of the handbook (i.e., the applied changes reflect
    different essence, and not simply restatements of the original 
    material), then the `x` component of the version number should be
    incremented (The new version number should be `(x + 1).0.0`).
### Semantic Versioning of Software
*   This is taken directly from `https://semver.org/`.
*   Let the semantic version number of the Assembler software be `x.y.z` for
    the present version $v$. A new change $C$ is applied to the software so
    that the new version is $v'$. The new version number of $v'$ can be any
    of thefollowing: `(x + 1).0.0`, `x.(y + 1).0`, or `x.y.(z + 1)`.
1.  MAJOR (`x`) version when you make incompatible API changes
2.  MINOR (`y`) version when you add functionality in a backward compatible
    manner
3.  PATCH (`z`) version when you make backward compatible bug fixes