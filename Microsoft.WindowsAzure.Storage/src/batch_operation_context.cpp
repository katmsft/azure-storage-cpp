// -----------------------------------------------------------------------------------------
// <copyright file="batch_operation_context.cpp" company="Microsoft">
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

#include "stdafx.h"
#include "was/service_client.h"
#include "wascore/resources.h"
#include "wascore/util.h"

namespace azure { namespace storage {

        void batch_operation_context::push_batch_request_body_for_batch_requests(utility::string_t& request_body, const utility::string_t & batch_id)
        {
            std::lock_guard<std::mutex> gurad(this->m_batch_request_vector_mutex);

            if (m_batch_requests.size() > 1000)
            {
                throw storage_exception(protocol::error_batch_size_too_huge, false);
            }

            for (size_t i = 0; i < m_batch_requests.size(); ++i)
            {
                batch_operation_context::push_batch_request_body_for_batch_request(request_body, batch_id, m_batch_requests[i], i);
            }
            core::write_boundary(request_body, utility::string_t(protocol::batch_boundary_name) + _XPLATSTR("_") + batch_id, true);
            return;
        }

        void batch_operation_context::push_batch_request_body_for_batch_request(utility::string_t& request_body, const utility::string_t & batch_id, const web::http::http_request & request, size_t content_id)
        {
            core::write_boundary(request_body, utility::string_t(protocol::batch_boundary_name) + _XPLATSTR("_") + batch_id);
            core::write_batch_required_headers(request_body, content_id);
            core::write_request_line(request_body, request.method(), request.request_uri().path() + request.request_uri().query());
            core::write_request_headers(request_body, request.headers());
            core::write_request_payload(request_body, request.body());
        }

}} // namespace azure::storage
