// -----------------------------------------------------------------------------------------
// <copyright file="blob_batch_results.h" company="Microsoft">
//    Copyright 2019 Microsoft Corporation
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//      http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
// </copyright>
// -----------------------------------------------------------------------------------------

#pragma once

#include "cpprest/asyncrt_utils.h"

namespace azure { namespace storage {

    class blob_batch_result
    {
    public:

        web::http::status_code get_status_code() const
        {
            return m_status_code;
        }

        void set_status_code(web::http::status_code code)
        {
            m_status_code = code;
        }

        std::unordered_map<utility::string_t, utility::string_t>& headers()
        {
            return m_headers;
        }

        const utility::string_t& get_body() const
        {
            return m_body;
        }

        void set_body(utility::string_t&& body)
        {
            m_body = std::move(body);
        }

    private:
        web::http::status_code m_status_code;
        std::unordered_map<utility::string_t, utility::string_t> m_headers;
        utility::string_t m_body;
    };

    class blob_batch_results
    {
    public:
        const utility::string_t& get_batch_id() const
        {
            return m_batch_id;
        }

        void set_batch_id(const utility::string_t& batch_id)
        {
            m_batch_id = batch_id;
        }

        const utility::string_t& get_request_id() const
        {
            return m_request_id;
        }

        void set_request_id(const utility::string_t& request_id)
        {
            m_request_id = request_id;
        }

        const utility::string_t& get_server_info() const
        {
            return m_server_info;
        }

        void set_server_info(const utility::string_t& server_info)
        {
            m_server_info = server_info;
        }

        std::vector<blob_batch_result>& batch_results()
        {
            return m_results;
        }

    private:
        std::vector<blob_batch_result> m_results;
        utility::string_t m_server_info;
        utility::string_t m_request_id;
        utility::string_t m_batch_id;
    };
}} // namespace azure::storage
