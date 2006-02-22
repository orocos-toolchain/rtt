#!/usr/bin/python
#
# A script to generate an .epk file from an existing package directory
# ecospkggen.py (c) 2003 Peter Soetens
#
# ***************************************************************************
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# ***************************************************************************/
"""
File : ecospkggen.py
"""


import os
import re
import sys
import string

from optparse import OptionParser

class Generator:
    "Contains all user given info for generating the package(s)"

    def __init__(self, db):
        self.db = db
        self.packages = []
        self.bin_exts = ['gif','jpg','png', 'dia','pdf', 'ps', 'eps'] # Add your own binary files extensions here
        if verbose:
            self.opt_flag = ' -v '
        else:
            self.opt_flag = ''

    def store_packages(self, given):
        self.packages = []
        for i in given:
            if self.db.alias_dict.has_key(i):
                self.packages.append(self.db.alias_dict[i])
            else:
                self.packages.append(i)
    

    # Create an entire configuration package
    def create_config(self, target, template, all_packages, version, recursive):
        self.store_packages(all_packages)
        self.version  = version
        
        self.init_workdir()
        self.copy_tgt(target)
        self.copy_tmp(template)
        for i in self.packages:
            self.copy_pkg(i,"current", version, recursive)
        self.finish_workdir( target+"-"+template )

    ##
    # Create one target and all given packages
    ##
    def create_tgt(self, target, all_packages, version, recursive) :
        self.store_packages(all_packages)
        self.version  = version
        
        self.init_workdir()

        self.copy_tgt(target)
        for i in self.packages :
            self.copy_pkg(i,"current", version, recursive)
            
        self.finish_workdir( target )

    ##
    # Create one template and all given packages
    ##
    def create_tmp(self, template, all_packages, version, recursive) :
        self.store_packages(all_packages)
        self.version  = version
        
        self.init_workdir()

        self.copy_tmp(template)
        for i in self.packages :
            self.copy_pkg(i,"current", version, recursive)
            
        self.finish_workdir( template )

    ##
    # Creates a package from all packages located at/within root_package.
    # The 'current ' version number is renamed to <version>
    #
    # @param root_package The package to be distributed
    # @param version      The common new version number
    # @param recursive    Recursive processing
    ##
    def create_pkg(self, root_package, version, recursive) :
        self.store_packages( [root_package ])
        self.version  = version
        
        self.init_workdir()
        self.copy_pkg(root_package, "current", version, recursive)
        self.finish_workdir(root_package)

    def copy_tgt(self, target) :
        if ( not quiet ):
            print "Producing target "+ target
        
        for t in self.db.targets:
            if target == t.name or target == t.alias :
                t.print_output(self.pkgadd)
                # copy/add each leaf-package of this target
                for p in t.packages :
                    if p not in self.packages :
                        self.copy_pkg( p, "current", self.version, False ) #only copy if not listed on command line

    # This function copies the template itself *and* all packages associated with the
    # template
    def copy_tmp(self, template ):
        if ( not quiet ):
            print "Producing template "+ template

        # copy/rename template file
        # change any occurence of 'current' with 'version number' within template
        os.makedirs(self.workdir+"/templates/"+template);
        os.system("sed -e s/current/"+self.version+"/ "+self.opt_flag+" "+self.db.repos+"/templates/"+template+"/current.ect >" + self.workdir +"/templates/"+template+"/"+self.version+".ect")

        # create an object containing all info.
        self.template = Template(self.db.repos+"/templates/"+template+"/current.ect") 
        
        # copy/add each leaf-package with version number
        # command-line specified packages override the ones in the template
        for i,j in self.template.packages:
            #print "Trying "+i+" version "+j
            if i not in self.packages :
                if j == "current":
                    pkg_vers = self.version
                else:
                    pkg_vers = j
                self.copy_pkg(i, j, pkg_vers, False) #only copy if not listed on command line
                #print "ok"

    def copy_pkg(self, root_package, rep_version, pkg_version, recursive) :
        if not quiet:
            print "Creating package "+ root_package+ " version "+pkg_version
        # copy/add each leaf-package
        # if pkg_version exists in the rep, take that one instead of rep_version...
        # this is a pretty sane thing to do and thus allows to create a package dir
        # first in the repository (and ecos.db !!), test it and then distribute it.
        somethingfound = 0;
        for i in self.db.packages:
            if (recursive and ( root_package in i.name or root_package in i.alias) ) or root_package == i.name or root_package == i.alias :
                somethingfound = 1;
                i.print_output(self.pkgadd)
                targetdir = self.workdir + "/" + i.dir +"/"+ pkg_version
                os.makedirs(targetdir)
                if os.access(self.db.repos + "/" + i.dir +"/"+pkg_version, os.F_OK) :
                    os.system("cp -a " + self.db.repos + "/" + i.dir +"/"+pkg_version+"/* "+ targetdir  )
                else:
                    os.system("cp -a " + self.db.repos + "/" + i.dir +"/"+rep_version+"/* "+ targetdir  )
        if somethingfound == 0:
            if not quiet:
                print "... no such package found in ecos.db !"
                    

    def init_workdir(self):
        self.workdir = "/tmp/ecospkggen." + str(os.getpid())
        os.mkdir(self.workdir)
        if licence != "":
            os.system("cp "+self.opt_flag+licence+" "+self.workdir+"/pkgadd.txt")
        # create new pkgadd.db file
        self.pkgadd = open( self.workdir+"/pkgadd.db",'w')

    def finish_workdir(self, base_name):
        self.pkgadd.close()
        # make the final distribution and remove/rename some files
        os.system("chmod -R +w " + self.workdir )
        # remove backup files etc
        os.system("find "+self.workdir+" -name \"*~\" -o  -name .svn -a -type d -o -name CVS -a -type d | xargs rm "+self.opt_flag+"-rf")
        for ext in self.bin_exts:
            os.system("find "+self.workdir+" -type f -name '*."+ ext + "' -exec mv {} {}.bin ';'")
        os.system("tar "+self.opt_flag+"-cz -f "+ self.db.repos + "/" +base_name+"-"+self.version+".epk -C " + self.workdir +" "+ string.join(os.listdir(self.workdir)) )
        os.system("rm "+self.opt_flag+"-rf "+self.workdir )


class EcosDB :
    "Our own db of found packages"

    def __init__(self, path= ".", file = "ecos.db") :
        self.rawtext = open( path +"/"+ file, 'r').read()
        self.repos   = path
        self.pkg_name_pat = re.compile(r"\s*package\s+(\S+)\s*{(.*?{.*?}.*?)}",re.S)
        self.tgt_name_pat = re.compile(r"\s*target\s+(\S+)\s*{(.*?{.*?}.*?{.*?}.*?)}",re.S)
        self.packages = [] # maybe use a dictionary instead of list
        self.targets  = [] # all targets in the db file
        self.alias_dict = {} # when an alias is given, we retrieve the packagename here.

    def parseDB(self):
        res = self.pkg_name_pat.findall(self.rawtext)
        for i in range(0,len(res)):
            tmppkg = Package( res[i][0], res[i][1] )
            self.packages.append(tmppkg)

        # build alias dictionary
        for i in self.packages:
            self.alias_dict[i.alias] = i.name

        res = self.tgt_name_pat.findall(self.rawtext)
        for i in range(0,len(res)):
            tmptgt = Target( res[i][0], res[i][1] )
            self.targets.append(tmptgt)

class Template:
    "A template defining packages and versions"
    def __init__(self, file):
        tf = open(file, 'r')
        tmp_pkg_pat = re.compile(r"package\s+(\w+)\s+(\S+)\s*;")
        self.raw_contents = tf.read()
        tf.close()
        self.packages = tmp_pkg_pat.findall(self.raw_contents) ## find all NAME, version pairs
        #print self.packages

    # Check if a given package is listed
    def have_pkg(self,name):
        for p in self.packages:
            if p[0].name == name:
                return True
        return False
            

class Target :
    "A target to be added to the pkgadd.db file"
    def __init__(self, _name, _contents) :
        self.name = _name
        self.raw_contents = _contents
        self.packages = []

        tgt_pkg_pat = re.compile(r"packages\s*{\s*(.*?)\s*}", re.S)
        tgt_alias_pat= re.compile(r"alias\s+{\s\"(.+?)\"\s+(.+?)\s*}")

        res = tgt_pkg_pat.search(_contents)
        self.packages = string.split( res.group(1), '\n')
        
        res = tgt_alias_pat.search(_contents)
        self.description = res.group(0)
        self.alias       = res.group(1)

    def print_output(self, _out=sys.stdout):
        _out.write("target " + self.name + " {")
        _out.write(self.raw_contents)
        _out.write("}\n")

    # Returns a list of all required packages for this target
    def get_packages():
        return self.packages

    # Check if a given package is listed
    def have_pkg(self,name):
        for p in self.packages:
            if p == name :
                return True
        return False
            

class Package :
    "The contents of an ecos package"
    def __init__(self, _name, _contents) :
        self.name = _name
        self.raw_contents = _contents

        # search patterns
        pkg_dir_pat  = re.compile(r"directory\s+(\S+)")
        pkg_alias_pat= re.compile(r"alias\s*{\s*\"(.+?)\"\s+(.+?)\s*}")
        pkg_script_pat= re.compile(r"script\s+(\S+)")

        # extract info
        pkg_match = pkg_dir_pat.search(_contents)
        if pkg_match != None:
            self.dir = pkg_match.group(1)
        else:
            print "Package "+ self.name +" has no directory entry !"
        alias_match = pkg_alias_pat.search(_contents)
        if alias_match != None:
            self.description, self.alias = alias_match.groups()
            self.alias = string.split(self.alias)[0] # XXX take only one alias into account
        else:
            print "Package "+ self.name +" has no alias entry !"
        script_match = pkg_script_pat.search(_contents)
        if script_match != None:
            self.script = script_match.group(1)
        else:
            print "Package "+ self.name +" has no script entry !"

    
    def print_output(self, _out=sys.stdout):
        _out.write("package " + self.name + " {")
        _out.write(self.raw_contents)
        _out.write("}\n")

# Main Script start

myusage = "\n    %prog [options] -V VERSION PACKAGES\n" \
          " Will result in PACKAGES being built separately, all having version VERSION.\n\n" \
          "    %prog [options] -t TARGET -V VERSION [ PACKAGES ]\n"\
          " Will result in a single package containing TARGET and all PACKAGES.\n\n" \
          "    %prog [options] -t TARGET -T TEMPLATE -V VERSION [ PACKAGES ]\n"\
          " Will result in a single package containing TARGET, TEMPLATE and all PACKAGES.\n" \
          "\n All PACKAGES listed on the command line will be taken from 'current' and override versions given in TARGET or TEMPLATE.\n" \
          " All packages listed in the TARGET and the TEMPLATE file will be included in the archive.\n\n" \
          "     use %prog --help \n" \
          "     for help on all options.\n"


parser = OptionParser( usage=myusage, version = "%prog 0.1")
parser.add_option("-R", "--repository", dest="path", default=".",
                  help="The path to the eCos repository [default='./']", metavar="PATH")
parser.add_option("-V", "--version_number", dest="version",
                  help="The version number of the generated packages", metavar="VERSION")
parser.add_option("-q", "--quiet",
                  action="store_true", dest="quiet", default=False,
                  help="don't print any messages to stdout")
parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", 
                  help="make lots of noise")
parser.add_option("-r", "--recursive",
                  action="store_true", dest="recursive", default=True,
                  help="Process packages recursively [default]")
parser.add_option("-S", "--single",
                  action="store_false", dest="recursive",
                  help="Do not process packages recursively")
parser.add_option("-L", "--licence",
                  action="store", dest="licence", metavar="LICENCE_FILE", default="",
                  help="Specify the licence to add to the package.")
parser.add_option("-t", "--target",
                  action="store", dest="targetname", metavar="TARGET_NAME", default="",
                  help="Specify a target. This will result in a single package which " \
                  "contains the target, its dependencies and all specified packages. ")
parser.add_option("-T", "--template",
                  action="store", dest="templatename", metavar="TEMPLATE_NAME", default="",
                  help="Specify a template. This will result in a single package which " \
                  "contains the template, its dependencies and all specified packages. ")

# interpret given options
(options, args) = parser.parse_args()

version = options.version
recursive = options.recursive
verbose = options.verbose
quiet   = options.quiet
licence = options.licence
targetname = options.targetname
templatename = options.templatename


if len(args) == 0 and len(targetname) == 0 and len(templatename) == 0 or version == None :
    parser.error ("Please specify the package name to generate ('alias' or XXXPKG_FULL_NAME)\n"\
                  " and/or a target and/or a template with a version number. (see options)")

# start application
mydb = EcosDB( options.path )

mydb.parseDB()

mygen = Generator(mydb)

# in case no target specified, build separate packages
if len(targetname) == 0  and len(templatename) == 0:
    packages = args
    for i in packages :
        mygen.create_pkg(i, version, recursive)
    sys.exit()

if len(templatename) !=0 and len(targetname) != 0 :
    mygen.create_config(targetname, templatename, args, version, recursive)
else:
    if len(templatename) != 0:
        mygen.create_tmp(templatename, args, version, recursive)
        sys.exit()
    if len(targetname) != 0:
        mygen.create_tgt(targetname, args, version, recursive)
        sys.exit()

               
    

        
        
        
    

