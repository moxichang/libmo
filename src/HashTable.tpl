#include <string>
#include <iostream>
#include <sstream>

#ifdef _INCLUDE_HASHTABLE_H_

#include "Exception.h"

template < class Type >
HashTable<Type>::HashTable(const size_t table_size_, float load_factor_)
	: table_size(table_size_), key_number(0), load_factor(load_factor_)
{
	table = new HashNode *[table_size_];

	for(size_t i = 0; i < table_size_; i++){
		table[i] = NULL;
	}

	rehash_size = (size_t)(table_size*load_factor);
}

template < class Type >
HashTable<Type>::HashTable(HashTable & ht_)
{
	this->table_size = 0;
	this->table = NULL;

	(*this) = ht_;
}

template < class Type >
HashTable<Type>::~HashTable()
{
	HashNode * node_tmp = NULL;

	for(size_t i=0; i< table_size; i++){
		for( HashNode *node = table[i]; node != NULL; ){
			node_tmp = node;
			node = node->next;
			delete node_tmp;
		}
	}

	delete [] table;
}

template < class Type >
void HashTable<Type>::Put(std::string key_, Type value_)
{

	if(key_.empty()){
		Throw("keys can not be empty",0);
	}

	int hash_key = this->Hash(key_.c_str(), key_.length());

	HashNode *node;
	//HashNode *new_node = new HashNode(key_, value_);

	if( (node = table[hash_key]) == NULL ){
		table[hash_key] = new HashNode(key_, value_);
	}
	else{
		while(node->next != NULL){
			if(node->key == key_){
				node->value = value_;
				return; //FIXME
			}
			node = node->next;
		}

		if(node->key == key_){
			node->value = value_;
			return; //FIXME
		}

		node->next = new HashNode(key_, value_);
	}

	key_number ++;

	if( key_number > rehash_size){
		//std::cout<<"Key Number:"<<key_number<<" Table Size:"<<this->table_size<<" Rehash Size:"<<this->rehash_size<<". Rehashing needed."<<std::endl;
		Rehash();
	}

	return;
}

template < class Type >
bool HashTable<Type>::Get(std::string key_, Type & value_)
{
	if(key_.empty()){
		Throw("keys can not be empty",0);
	}

	int hash_key = this->Hash(key_.c_str(), key_.length());

	bool found = false;
	for( HashNode *node = table[hash_key]; node != NULL; node = node->next){
		if(key_ == node->key){
			value_ = node->value;
			found = true;
			break;
		}
	}

	return found;
}

template < class Type >
Type& HashTable<Type>::operator [] (std::string key_)
{
	if(key_.empty()){
		Throw("keys can not be empty",0);
	}

	int hash_key = this->Hash(key_.c_str(), key_.length());
	
	for( HashNode *node = table[hash_key]; node != NULL; node = node->next){
		if(key_ == node->key){
			return node->value;
		}
	}	

	Throw("given key is not exist in hashtable",0);
	
	// FIXME!!! Hack for eliminate the complier waring
	//static Type value = 0;
	return null_object;
}

template < class Type >
HashTable<Type> & HashTable<Type>::operator = (HashTable& ht_)
{

	HashNode ** old_table = table;
	size_t old_size = table_size;

	table_size = ht_.table_size;
	key_number = 0;
	load_factor = ht_.load_factor;
	rehash_size = ht_.rehash_size;

	table = new HashNode *[ht_.table_size];
	for(size_t i=0; i < ht_.table_size; i++){
		table[i] = NULL;
	}

	for(size_t i=0; i < ht_.table_size; i++){
		for( HashNode *node = ht_.table[i]; node != NULL; node = node->next){
			this->Put(node->key,node->value);
		}
	}

	for(size_t i=0; i < old_size; i++){
		
		HashNode *np = old_table[i];
		HashNode *np_next;

		while(np != NULL){
			np_next = np->next;
			delete np;			
			np = np_next;
		}

		old_table[i] = NULL;
	}

	delete [] old_table;
	
	return ht_;
}

template < class Type >
bool HashTable<Type>::Remove(std::string key_)
{

	if(key_.empty()){
		Throw("keys can not be empty",0);
	}

	int hash_key = this->Hash(key_.c_str(), key_.length());
	
	HashNode *node = NULL;
	HashNode *t_node = NULL;

	bool found = false;

	node = table[hash_key];

	//std::cout<<"deleteing:"<<key_<<"\thash_key:"<<hash_key<<std::endl;

	// in the beginning of the collision queue
	if( node->key == key_){
		table[hash_key] = node->next;
		--key_number;
		delete node;

		found = true;
	}
	else{
		for( ; node != NULL; node = node->next){
			// in the middle of the collision queue
			if(node->next != NULL && node->next->key == key_){			
				t_node = node->next;
				node->next = node->next->next;
				--key_number;
				delete t_node;

				found = true;

				break;
			}
		}
	}

	return found;
}

template < class Type >
size_t HashTable<Type>::Clear()
{
	size_t num = 0;

	for(size_t i=0; i < table_size; i++){
		
		HashNode *np = table[i];
		HashNode *np_next;

		while(np != NULL){
			np_next = np->next;
			delete np;			
			num ++;
			np = np_next;
		}

		table[i] = NULL;
	}	

	key_number = 0;

	return num;
}

template < class Type >
bool HashTable<Type>::Contains(std::string key_)
{

	if(key_.empty()){
		Throw("keys can not be empty",0);
	}

	int hash_key = this->Hash(key_.c_str(), key_.length());
	
	bool found = false;
	for( HashNode *node = table[hash_key]; node != NULL; node = node->next){
		if(key_ == node->key){
			found = true;
			break;
		}
	}	

	return found;
}


template < class Type >
bool HashTable<Type>::IsEmpty()
{
	return key_number == 0 ? true : false;
}

template < class Type >
int HashTable<Type>::Size()
{
	return key_number;
}

template < class Type >
void HashTable<Type>::PrintAsTree()
{

	std::stringstream sf;	
	sf<<table_size;
	size_t t_len = sf.str().length() + 1;

	std::cout<<"----------------------------------------"<<std::endl;
	for(size_t i = 0; i < this->table_size; i++){
		std::cout<<i;
		int j = 0;
		for( HashNode *node = table[i]; node != NULL; node = node->next){
			
			sf.str("");
			sf<<i;
			int i_len = sf.str().length() + 1;

			if( j != 0 ){
				for(size_t k=0; k < t_len; k++){
					std::cout<<" ";
				}
				if(node->next == NULL)
					std::cout<<"`";
				else
					std::cout<<"|";
			}
			else{
				for(size_t k=0; k < 1+t_len-i_len; k++){
					std::cout<<"-";
				}
				std::cout<<"+";
			}

			std::cout<<"---"<<j<<": "<<""<<node->key<<" => "<<node->value<<std::endl;
			j++;
		}
		std::cout<<std::endl;
	}
	std::cout<<std::endl<<"Buckets:"<<table_size<<" Keys:"<<key_number<<std::endl;
	std::cout<<"----------------------------------------"<<std::endl;
}

template < class Type >
void HashTable<Type>::PrintAsList()
{

	for(size_t i = 0; i < this->table_size; i++){
		for( HashNode *node = table[i]; node != NULL; node = node->next){
			std::cout<<node->key<<" = '"<<node->value<<"'"<<std::endl;
		}
	}
}

template < class Type>
void HashTable<Type>::Rehash()
{
	HashNode **old_table;
	size_t old_table_size;

	old_table = this->table;
	old_table_size = this->table_size;	

	this->table = new HashNode *[table_size*2];
	this->table_size = table_size*2;
	for(size_t i=0;i<table_size; i++){
		table[i] = NULL;
	}

	key_number = 0;
	this->rehash_size = (size_t)(table_size*load_factor);

	HashNode * node_tmp = NULL;
	for(size_t i=0;i<old_table_size;i++){
		for( HashNode *node = old_table[i]; node != NULL; ){
			this->Put(node->key,node->value);
			
			node_tmp = node;
			node = node->next;
			delete node_tmp;
		}
	}

	delete [] old_table;
}

template < class Type >
unsigned int HashTable<Type>::Hash(const char *key_, unsigned int key_len_)
{
	// Jenkins One-at-a-time hash
	// http://www.burtleburtle.net/bob/hash/doobs.html
	unsigned int hash = 0;
	unsigned int i;

	for (i = 0; i < key_len_; i++) {
		hash += key_[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash%table_size;
}

#endif /* _INCLUDE_HASHTABLE_H_ */
