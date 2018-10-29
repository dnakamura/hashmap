#pragma once
#include <OMR/GC/ScanResult.hpp>
#include <OMR/GC/RefSlotHandle.hpp>

#include <iostream>
#include "Object.hpp"
#include "HashTable.hpp"

namespace OMRClient{
    namespace GC{
class ObjectScanner{
    public:
		ObjectScanner() = default;

		ObjectScanner(const ObjectScanner&) = default;

        template <typename VisitorT>
		OMR::GC::ScanResult
		start(VisitorT&& visitor, Object* node, std::size_t bytesToScan = SIZE_MAX) noexcept {
            std::cout << "Scanning "<< node ;
            switch(node->kind()){
               
                case Object::Kind::STRING:
                    std::cout << "<STRING>" << Size(node) << std::endl;
                    return { Size(node) , true };

                case Object::Kind::HASHTABLE:
                {
                    std::cout << "<HASHTABLE>\n";
                    HashTable *ht = static_cast<HashTable*>(node);
                    if(ht->backingArray_){
                        std::cout << "  edge " << ht->backingArray_ << std::endl;
                        visitor.edge(ht, OMR::GC::RefSlotHandle(reinterpret_cast<Object**>(&(ht->backingArray_))));
                        
                    }
                    return { sizeof(HashTable) , true };
                }
                case Object::Kind::REF_ARRAY:
                    std::cout << "<REFARRAY>\n";
                    return scanRefArray(visitor, static_cast<RefArray*>(node));
            }
			assert(0);
            return {0, true};
		}

        template <typename VisitorT>
		OMR::GC::ScanResult
		scanRefArray(VisitorT&& visitor, RefArray* arr){

            for(size_t i = 0 ; i < arr->count(); ++i){
                HashTable::Entry & entry = arr->entries_[i];
                if(entry.key.IsObject()){
                    if(entry.key.object_){
                        std::cout << "  edge " << entry.key.object_ << std::endl;
                        visitor.edge(arr, OMR::GC::RefSlotHandle(&entry.key.object_));
                    }
                }
                if(entry.value.IsObject()){
                    if(entry.value.object_){
                        std::cout << "  edge " << entry.value.object_ << std::endl;
                        visitor.edge(arr, OMR::GC::RefSlotHandle(&entry.value.object_));
                    }
                }
            }
            return {arr->size(), true};
        }
    
		template <typename VisitorT>
		OMR::GC::ScanResult
		resume(VisitorT&& visitor, std::size_t bytesToScan = SIZE_MAX) noexcept {
			return { 0, true};
		}


};
    }}
