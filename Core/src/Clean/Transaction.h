/** \file Core/Transaction.h
**/

#ifndef CLEAN_TRANSACTION_H
#define CLEAN_TRANSACTION_H

#include <chrono>
#include <cstdint>
#include <cstddef>

namespace Clean 
{
   /** @brief Defines a Transaction, which is a Command that is executed asynchroneously or ignored
    *  if a given timelapse is reached. 
    *
    * Transactions are used to make some objects do some commands asynchroneously via an alternative update
    * method, like Mesh::update. Transactions are fully customized by the object that uses them, like Mesh which
    * defines a bunch of kMeshTransaction* constants. 
    *
   **/
   class Transaction final 
   {
   public:
       using Clock = std::chrono::high_resolution_clock; 
       
   private:
       //! @brief The type of the transaction. 
       std::uint8_t type_;
       
       //! @brief The data for the transaction.
       void* data_;
       
       //! @brief Time untill the Transaction is invalidated.
       Clock::time_point tpoint_;
       
   public:
       
       Transaction();
       Transaction(Transaction&&) = default;
       
       Transaction(Transaction const&) = delete;
       Transaction operator = (Transaction const&) = delete;
       
       /*! @brief Constructs a new Transaction. */
       Transaction(std::uint8_t t, void* d, Clock::time_point const& untill = Clock::now());
       
       /*! @brief Destruct the transaction and delete the data, if not null. */
       ~Transaction();
       
       /*! @brief Returns the Transaction's type. */
       std::uint8_t type() const;
       
       /*! @brief Returns the Transaction's data. May be null. */
       void* data();
       
       /*! @brief Returns true if tpoint_ is still > to Clock::now(). */
       bool valid() const;
   };
}

#endif // CLEAN_TRANSACTION_H
