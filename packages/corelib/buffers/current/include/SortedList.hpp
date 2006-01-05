#ifndef ORO_SORTED_LIST_HPP
#define ORO_SORTED_LIST_HPP

#include <os/CAS.hpp>
#include <boost/shared_ptr.hpp>
#include "MemoryPool.hpp"

namespace ORO_CoreLib
{
    /**
     * A single-linked sorted list algorithm invented by Timothy
     * L. Harris. This implementation is only for 32bit computers. You may \b not insert
     * the same item more than once.
     *
     * The difference between this implementation and Harris' is that we
     * use a self-invented memory pool (MemoryPool) for lock-free/hard real-time memory
     * management and an apply() function to manipulate the data within the SortedList.
     *
     * @param DataType_ Must be a pointer type or integer or any
     * object which has or for which you defined an operator<() and operator==()
     * since the list is sorted.
     */
    template<class DataType_>
    class SortedList
    {
    public:
        typedef DataType_ DataType;
    protected:
        struct NodeType
        {
            typedef NodeType* NodeType_sptr;
            DataType key;
            NodeType_sptr next;
            
            NodeType(const DataType& data)
                : key(data)
            {}

            NodeType()
                : key()
            {}
        };

        /**
         * Node type.
         */
        typedef NodeType Node;
        /**
         * Node shared pointer type.
         */
        typedef typename NodeType::NodeType_sptr Node_sptr;
    
        MemoryPool<Node> mpool;

        Node_sptr head;
        Node_sptr tail;

        Node_sptr search(const DataType& search_key, Node_sptr& left_node)
        {
            Node_sptr left_node_next, right_node;
        search_again:
            do {
                Node_sptr t = this->head;
                Node_sptr t_next = this->head->next;

                do {
                    if (!is_marked_reference(t_next)) {
                        left_node = t;
                        left_node_next = t_next;
                    }
                    t = get_unmarked_reference(t_next);
                    if (t == this->tail)
                        break;
                    t_next = t->next;
                } while (is_marked_reference(t_next) || (t->key < search_key));
                right_node = t;
                
                if (left_node_next == right_node)
                    if ((right_node != this->tail) && is_marked_reference(right_node->next))
                        goto search_again;
                    else
                        return right_node;

                if (ORO_OS::CAS(&(left_node->next), left_node_next, right_node)) {
                    mpool.deallocate( get_unmarked_reference(left_node_next) );
                    if ((right_node != this->tail) && is_marked_reference(right_node->next))
                        goto search_again;
                    else
                        return right_node;
                }
            } while (true);
        }

        bool is_marked_reference(Node* n)
        {
            unsigned int v = reinterpret_cast<unsigned int>(n);
            return (v%2) == 1;
        }

        Node* get_unmarked_reference(Node* n)
        {
            unsigned int v = reinterpret_cast<unsigned int>(n);
            return reinterpret_cast<Node*>( v & -2 ); // is ..11110
        }

        Node* get_marked_reference(Node* n)
        {
            unsigned int v = reinterpret_cast<unsigned int>(n);
            return reinterpret_cast<Node*>( v | 1);  // is ...00001
        }
    public:

        /**
         * Create an empty list.
         */
        SortedList()
        {
            mpool.reserve();
            mpool.reserve();
            head = mpool.allocate();
            tail = mpool.allocate();

            head->next = tail;
        }

        ~SortedList()
        {
            mpool.deallocate(head);
            mpool.deallocate(tail);
        }

        /**
         * Reserve memory for one list item.
         */
        void reserve(size_t n = 1)
        { size_t i(0); while (i++ < n) mpool.reserve(); }

        /**
         * Free memory for one list item.
         */
        void shrink(size_t n = 1)
        { size_t i(0); while (i++ < n) mpool.shrink(); }

        /**
         * Inspect if the list is empty.
         */
        bool empty() const
        {
            return head->next == tail;
        }

        /**
         * Insert a new node.
         * @param key A not yet inserted key object.
         * @return true on success, false if \a key already present in list.
         */
        bool insert(const DataType& key)
        {
            Node_sptr new_node( mpool.allocate() );
            Node_sptr right_node, left_node;

            new_node->key = key;
            do {
                right_node = this->search(key, left_node);
                if ((right_node != tail) && (right_node->key == key )) {
                    mpool.deallocate( new_node );
                    return false;
                }
                new_node->next = right_node;
                if (ORO_OS::CAS(&(left_node->next), right_node, new_node))
                    return true;
            } while (true);
        }
        
        /**
         * Erase a node.
         * @param key An inserted key object.
         * @return true on success, false if \a key not present in list.
         */
        bool erase(const DataType& search_key) 
        {
            Node_sptr right_node, right_node_next, left_node;

            do {
                right_node = search(search_key, left_node);
                if (( right_node == tail) || !(right_node->key == search_key) )
                    return false;
                right_node_next = right_node->next;
                if (!is_marked_reference(right_node_next))
                    if (ORO_OS::CAS( &(right_node->next), right_node_next, get_marked_reference(right_node_next)))
                        break;
            } while(true);
            if (!ORO_OS::CAS(&(left_node->next), right_node, right_node_next))
                right_node = search(right_node->key, left_node);
            else
                mpool.deallocate( get_unmarked_reference(right_node) );
            return true;
        }

        /**
         * Check if a node is present.
         * @param key A key object.
         * @return true if present, false if \a key not present in list.
         */
        bool hasKey(const DataType& search_key)
        {
            Node_sptr right_node, left_node;

            right_node = search( search_key, left_node);
            if ((right_node == tail) || !(right_node->key == search_key))
                return false;
            else
                return true;
        }

        /**
         * Applies a function to all elements.
         * For example:
         @verbatim
         SortedList<CallBackInterface*> sl;
         // insert elements...
         // call the 'callback' function on each element:
         sl.apply( boost::bind(&CallBackInterface::callback, _1) );
         @endverbatim
         * @param f A Function object to apply to each element.
         * @warning You may not apply functions to the elements which
         * change the relative results of operator<() or operator==()
         * of the contained elements.
         */
        template<class Function>
        void applyOnData(const Function& f )
        {
            // start at head, go until tail. This is similar to search().
            Node_sptr t = this->head;
            Node_sptr t_next = this->head->next;

            // Marked fields may be traversed !
            // thus go on until you hit the tail.
            do {
                // need to unmark always to be sure.
                t = get_unmarked_reference(t_next);
                if (t == this->tail)
                    return;
                if (!is_marked_reference(t_next)) {
                    // The applying conseptually starts here. If t_next is now concurrently erased,
                    // f() was already in progress of execution, so it continues.
                    f( t->key );
                }
                t_next = t->next;
            } while ( true );

        }

        /**
         * This is a safer version of apply() with respect to
         * concurrent erasure/insertion. First a copy of the
         * data is made, then the node is checked if it is still
         * present, if so the function is applied on the copied data, thus
         * the data is for sure not corrupted.
         */
        template<class Function>
        void applyOnCopy(const Function& f )
        {
            // start at head, go until tail. This is similar to search().
            Node_sptr t;
            Node_sptr t_next = this->head->next;

            // Marked fields may be traversed !
            // thus go on until you hit the tail.
            do {
                // need to unmark always to be sure.
                t = get_unmarked_reference(t_next);
                if (t == this->tail)
                    return;
                // make the copy
                DataType d(t->key);
                if (!is_marked_reference(t_next)) {
                    // The applying conseptually starts here. If t_next is now concurrently erased,
                    // f() was already in progress of execution, so it continues.
                    f( d );
                }
                t_next = t->next;
            } while ( true );

        }
    };

}

#endif
