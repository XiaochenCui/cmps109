// $Id: inode.cpp,v 1.12 2014-07-03 13:29:57-07 - - $

#include <iostream>
#include <stdexcept>

using namespace std;

#include "debug.h"
#include "inode.h"

int inode::next_inode_nr {1};

inode::inode(inode_t init_type):
   inode_nr (next_inode_nr++), type (init_type)
{
   switch (type) {
      case PLAIN_INODE:
           contents = make_shared<plain_file>();
           break;
      case DIR_INODE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

file_base_ptr inode::get_contents(){
    return contents;
}

plain_file_ptr plain_file_ptr_of (file_base_ptr ptr) {
   plain_file_ptr pfptr = dynamic_pointer_cast<plain_file> (ptr);
   if (pfptr == nullptr) throw invalid_argument ("plain_file_ptr_of");
   return pfptr;
}

directory_ptr directory_ptr_of (file_base_ptr ptr) {
   directory_ptr dirptr = dynamic_pointer_cast<directory> (ptr);
   if (dirptr == nullptr) throw invalid_argument ("directory_ptr_of");
   return dirptr;
}

size_t plain_file::size() const {
   size_t size {0};
   size = data.size();
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   data = words;
}

size_t directory::size() const {
   size_t size {0};
   size = dirents.size();
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir(const string& dirname) {
    DEBUGF ('i', dirname);
    if (dirents.find(dirname) != dirents.end())
        throw yshell_exn ("dirname exists");
    inode_ptr parent = dirents["."];
    inode_ptr dirnode = make_shared<inode>(DIR_INODE);
    directory_ptr dir = directory_ptr_of(dirnode->get_contents());
    dir->set_parent_child(parent, dirnode);
    dirents.insert(make_pair(dirname, dirnode));
    return dirnode;
}

inode_ptr directory::mkfile (const string& filename) {
    DEBUGF ('i', filename);
    if (dirents.find(filename) != dirents.end())
        throw yshell_exn ("filename exists");
    inode_ptr file = make_shared<inode>(PLAIN_INODE);
    dirents.insert(make_pair(filename, file));
    return file;
}

void directory::set_root(inode_ptr root) {
    dirents.insert(make_pair(".", root));
    dirents.insert(make_pair("..", root));
}

void directory::set_parent_child(inode_ptr parent, inode_ptr child) {
    dirents.insert(make_pair("..", parent));
    dirents.insert(make_pair(".", child));
}

inode_state::inode_state() {
    root = make_shared<inode>(DIR_INODE);
    cwd = root;
    directory_ptr_of(root->contents)->set_root(root);
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt << "\"");
}



ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

ostream& operator<< (ostream& out, const directory& dir) {
   out << dir.pwd(); 
   return out;
}
