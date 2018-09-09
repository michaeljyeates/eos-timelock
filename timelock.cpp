#include <eosiolib/eosio.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/asset.hpp>
#include <string>
#include <stdio.h>

using namespace eosio;
using namespace std;

using eosio::print;

class timelock : public eosio::contract {
  public:
      using contract::contract;

      /// @abi action 
      void transfer(name from,
                      name to,
                      asset quantity,
                      string memo ) {

          auto size = transaction_size();
          char* buffer = (char*) alloca(size);
          uint32_t read = read_transaction( buffer, size );
          eosio_assert( size == read, "read_transaction failed");

          transaction trx = eosio::unpack<transaction>(buffer, size);

          unsigned_int delay_sec = trx.delay_sec;
          unsigned_int required_delay = 5;
          // check that the delay satisfies the conditions


          char* dbg_buffer = (char*) alloca(100);
          int n;
          n = sprintf (dbg_buffer, "Delay was %d require %d", delay_sec, required_delay);
          eosio_assert( n < 100, "Debug print alloc fail" );

          print_f((const char *)dbg_buffer);

          eosio_assert( delay_sec >= required_delay, "Delay is not satisfied" );

          print("Delay was satisfied, tokens sent");
      }
};

#define EOSIO_ABI_EX(TYPE, MEMBERS) \
extern "C" { \
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
      if( action == N(onerror)) { \
         /* onerror is only valid if it is for the "eosio" code account and authorized by "eosio"'s "active permission */ \
         eosio_assert(code == N(eosio), "onerror action's are only valid from the \"eosio\" system account"); \
      } \
      auto self = receiver; \
      if( code == self || action == N(transfer) ) { \
         TYPE thiscontract( self ); \
         switch( action ) { \
            EOSIO_API( TYPE, MEMBERS ) \
         } \
         /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
      } \
   } \
}

EOSIO_ABI_EX(timelock, (transfer))
