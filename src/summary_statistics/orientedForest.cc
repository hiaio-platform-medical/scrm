/*
 * scrm is an implementation of the Sequential-Coalescent-with-Recombination Model.
 * 
 * Copyright (C) 2013, 2014 Paul R. Staab, Sha (Joe) Zhu and Gerton Lunter
 * 
 * This file is part of scrm.
 * 
 * scrm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "orientedForest.h"

void OrientedForest::calculate(const Forest &forest) {
  if (forest.model().recombination_rate() == 0.0) {
    output_buffer_ << "{" ;
    generateTree(forest);
    output_buffer_ << "}\n";  
  } else {
    if (forest.calcSegmentLength(forest.model().finite_sites()) == 0.0) return;
    output_buffer_ << "{" ;
    output_buffer_ << "\"duration\":" << forest.calcSegmentLength(forest.model().finite_sites()) << ", " ;
    generateTree(forest);
    output_buffer_ << "}\n";  
  }
}

void OrientedForest::printLocusOutput(std::ostream &output) {
  output << output_buffer_.str();  
  output_buffer_.str("");
  output_buffer_.clear();
}

/**
 * @brief Prints the entire local tree as in oriented forest format, with JSON annotation
 *
 * @param forest 
 *
 * @return STR = ' FLT, "pi" : [INT, INT, ... ], "height" : [FLT, FLT, ... ] '
 * Note space doesn't mean anything
 */
void OrientedForest::generateTree(const Forest &forest) {
  this->init_OF_label( forest );
  this->update_OF_label( forest );
    
  output_buffer_ << "\"pi\":[" ;
  for ( size_t i = 0 ; i < this->OF_labels.size() ; i++){
    output_buffer_ << this->OF_labels[i] <<  ((i < this->OF_labels.size()-1) ? ",":"" );
  }
  output_buffer_ << "], " ;

  output_buffer_ << "\"heights\":[" ;
  for ( size_t i = 0 ; i < this->heights.size() ; i++){
    output_buffer_ << this->heights[i] <<  ((i < this->heights.size()-1) ? ",":"" );
  }
  output_buffer_ << "]" ;
}


void OrientedForest::update_OF_label( const Forest &forest ){
  for (auto it = forest.getNodes()->iterator(); it.good(); ++it) {
    if ( (*it) == forest.local_root() )  {
        this->heights[(*it)->OF_label()-1] = (*it)->height();
        //cout << (*it)->OF_label() << "("<< 0 << ")" << ",";
    }
    if ( !(*it)->local() ) continue;
    if ( !(*it)->in_sample() && ( (*it)->second_child() == NULL || !( (*it)->second_child()->local())  || ! ((*it)->first_child()->local() )) ) continue;

    Node* tmp_parent = (*it)->parent();
    while ( ( tmp_parent->second_child() == NULL || !( tmp_parent->second_child()->local())  || !(tmp_parent->first_child()->local() ))  ){
        tmp_parent = tmp_parent->parent();
        }

    if ( (*it)->in_sample() && tmp_parent->OF_label() == 0 ) {
        this->tmp_label++;
        tmp_parent->set_OF_label( this->tmp_label );
        //cout << (*it)->OF_label() << "("<< tmp_parent->OF_label() << ")" << ",";
    }
    else if (  (*it)->in_sample()  ){
        if (tmp_parent == forest.local_root() ) this->tmp_label++;
        //cout << (*it)->OF_label() << "("<< tmp_parent->OF_label() << ")" << ",";
    }
    else if ( tmp_parent->OF_label() == 0 ){ 
        this->tmp_label++;
        tmp_parent->set_OF_label( this->tmp_label );
        //cout << (*it)->OF_label() << "("<< tmp_parent->OF_label() << ")" << ",";
    }
    //else {
        //cout << (*it)->OF_label() << "("<< tmp_parent->OF_label() << ")" << ",";
    //}
    this->OF_labels[(*it)->OF_label()-1] = tmp_parent->OF_label();
    this->heights[(*it)->OF_label()-1] = (*it)->height();
  }
  
  //cout<<endl;
  //for ( size_t i = 0 ; i < this->OF_labels.size() ; i++){
    //cout<<i+1<<"(" << this->OF_labels[i]  <<") " ;
  //}
  //cout<<endl;
  
  //for (auto it = forest.getNodes()->iterator(); it.good(); ++it) {
    //if ( (*it) == forest.local_root() )  {
        //this->heights[(*it)->OF_label()-1] = (*it)->height();
    //}
    //if ((*it)->OF_label()>0) this->heights[(*it)->OF_label()-1] = (*it)->height();
  //}
  
}

void OrientedForest::init_OF_label( const Forest &forest ){
  this->OF_labels.clear();
  this->heights.clear();
  size_t n = 0;
  //cout << "number of nodes:"<<endl;
  for (auto it = forest.getNodes()->iterator(); it.good(); ++it) {
    // Include the local root
    if ( (*it) == forest.local_root() ) {
      this->OF_labels.push_back( (size_t)0 ); 
      this->heights.push_back( (double)0 );
    }
    // Exclude all the non-local nodes
    if ( !(*it)->local() ) continue;
    // Exclude all the intermedia local nodes ( nodes which only have one child, 1 parent)
    if ( !(*it)->in_sample() && ( (*it)->second_child() == NULL || !( (*it)->second_child()->local())  || ! ((*it)->first_child()->local() )) ) continue;
    Node* tmp_parent = (*it)->parent();
    tmp_parent->set_OF_label( (size_t)0 );
    this->OF_labels.push_back( (size_t)0 );
    this->heights.push_back( (double)0 );
    //cout << n++<< " "<<(*it)<<" " <<tmp_parent<<endl;
  }
  //cout << n++<< " " << forest.local_root()<<endl;
    // Initialize the first parent node to the first tip node
  this->tmp_label = forest.model().sample_size();

}