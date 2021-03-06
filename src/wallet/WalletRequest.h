// Copyright (c) 2012-2014, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <deque>
#include <functional>
#include <memory>

#include "INode.h"

#include "WalletSynchronizationContext.h"
#include "WalletSendTransactionContext.h"
#include "WalletEvent.h"

namespace CryptoNote {

class WalletRequest
{
public:
  typedef std::function<void (std::deque<std::shared_ptr<WalletEvent> >& events, boost::optional<std::shared_ptr<WalletRequest> >& nextRequest, std::error_code ec)> Callback;

  virtual ~WalletRequest() {};

  virtual void perform(INode& node, std::function<void (WalletRequest::Callback, std::error_code)> cb) = 0;
};

class WalletGetNewBlocksRequest: public WalletRequest
{
public:
  WalletGetNewBlocksRequest(const std::list<crypto::hash>& knownBlockIds, std::shared_ptr<SynchronizationContext> context, Callback cb) : m_ids(knownBlockIds), m_context(context), m_cb(cb) {};
  virtual ~WalletGetNewBlocksRequest() {};

  virtual void perform(INode& node, std::function<void (WalletRequest::Callback, std::error_code)> cb)
  {
    node.getNewBlocks(std::move(m_ids), std::ref(m_context->newBlocks), std::ref(m_context->startHeight), std::bind(cb, m_cb, std::placeholders::_1));
  };

private:
  std::shared_ptr<SynchronizationContext> m_context;
  std::list<crypto::hash> m_ids;
  Callback m_cb;
};

class WalletGetTransactionOutsGlobalIndicesRequest: public WalletRequest
{
public:
  WalletGetTransactionOutsGlobalIndicesRequest(const crypto::hash& transactionHash, std::vector<uint64_t>& outsGlobalIndices, Callback cb) : m_hash(transactionHash), m_outs(outsGlobalIndices), m_cb(cb) {};
  virtual ~WalletGetTransactionOutsGlobalIndicesRequest() {};

  virtual void perform(INode& node, std::function<void (WalletRequest::Callback, std::error_code)> cb)
  {
    node.getTransactionOutsGlobalIndices(m_hash, std::ref(m_outs), std::bind(cb, m_cb, std::placeholders::_1));
  };

private:
  crypto::hash m_hash;
  std::vector<uint64_t>& m_outs;
  Callback m_cb;
};

class WalletGetRandomOutsByAmountsRequest: public WalletRequest
{
public:
  WalletGetRandomOutsByAmountsRequest(const std::vector<uint64_t>& amounts, uint64_t outsCount, std::shared_ptr<SendTransactionContext> context, Callback cb) :
    m_amounts(amounts), m_outsCount(outsCount), m_context(context), m_cb(cb) {};

  virtual ~WalletGetRandomOutsByAmountsRequest() {};

  virtual void perform(INode& node, std::function<void (WalletRequest::Callback, std::error_code)> cb)
  {
    node.getRandomOutsByAmounts(std::move(m_amounts), m_outsCount, std::ref(m_context->outs), std::bind(cb, m_cb, std::placeholders::_1));
  };

private:
  std::vector<uint64_t> m_amounts;
  uint64_t m_outsCount;
  std::shared_ptr<SendTransactionContext> m_context;
  Callback m_cb;
};

class WalletRelayTransactionRequest: public WalletRequest
{
public:
  WalletRelayTransactionRequest(const cryptonote::Transaction& tx, Callback cb) : m_tx(tx), m_cb(cb) {};
  virtual ~WalletRelayTransactionRequest() {};

  virtual void perform(INode& node, std::function<void (WalletRequest::Callback, std::error_code)> cb)
  {
    node.relayTransaction(m_tx, std::bind(cb, m_cb, std::placeholders::_1));
  }

private:
  cryptonote::Transaction m_tx;
  Callback m_cb;
};

} //namespace CryptoNote
