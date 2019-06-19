// -----------------------------------------------------------------------------------------
// <copyright file="blob_response_parsers.cpp" company="Microsoft">
//    Copyright 2013 Microsoft Corporation
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

#include "wascore/protocol.h"
#include "wascore/constants.h"
#include "cpprest/asyncrt_utils.h"

namespace azure { namespace storage { namespace protocol {

    cloud_blob_container_properties blob_response_parsers::parse_blob_container_properties(const web::http::http_response& response)
    {
        cloud_blob_container_properties properties;
        properties.m_etag = parse_etag(response);
        properties.m_last_modified = parse_last_modified(response);
        properties.m_lease_status = parse_lease_status(response);
        properties.m_lease_state = parse_lease_state(response);
        properties.m_lease_duration = parse_lease_duration(response);
        properties.m_public_access = parse_public_access_type(response);
        return properties;
    }

    blob_type blob_response_parsers::parse_blob_type(const utility::string_t& value)
    {
        if (value == header_value_blob_type_block)
        {
            return blob_type::block_blob;
        }
        else if (value == header_value_blob_type_page)
        {
            return blob_type::page_blob;
        }
        else if (value == header_value_blob_type_append)
        {
            return blob_type::append_blob;
        }
        else
        {
            return blob_type::unspecified;
        }
    }

    standard_blob_tier blob_response_parsers::parse_standard_blob_tier(const utility::string_t& value)
    {
        if (value == header_value_access_tier_hot)
        {
            return standard_blob_tier::hot;
        }
        else if (value == header_value_access_tier_cool)
        {
            return standard_blob_tier::cool;
        }
        else if (value == header_value_access_tier_archive)
        {
            return standard_blob_tier::archive;
        }
        else
        {
            return standard_blob_tier::unknown;
        }
    }

    premium_blob_tier blob_response_parsers::parse_premium_blob_tier(const utility::string_t& value)
    {
        if (value == header_value_access_tier_p4)
        {
            return premium_blob_tier::p4;
        }
        else if (value == header_value_access_tier_p6)
        {
            return premium_blob_tier::p6;
        }
        else if (value == header_value_access_tier_p10)
        {
            return premium_blob_tier::p10;
        }
        else if (value == header_value_access_tier_p20)
        {
            return premium_blob_tier::p20;
        }
        else if (value == header_value_access_tier_p30)
        {
            return premium_blob_tier::p30;
        }
        else if (value == header_value_access_tier_p40)
        {
            return premium_blob_tier::p40;
        }
        else if (value == header_value_access_tier_p50)
        {
            return premium_blob_tier::p50;
        }
        else if (value == header_value_access_tier_p60)
        {
            return premium_blob_tier::p60;
        }
        else
        {
            return premium_blob_tier::unknown;
        }
    }

    utility::size64_t blob_response_parsers::parse_blob_size(const web::http::http_response& response)
    {
        auto& headers = response.headers();
        utility::string_t value;

        if (headers.match(web::http::header_names::content_range, value))
        {
            auto slash = value.find(_XPLATSTR('/'));
            value = value.substr(slash + 1);
            return utility::conversions::details::scan_string<utility::size64_t>(value);
        }

        if (headers.match(ms_header_blob_content_length, value))
        {
            return utility::conversions::details::scan_string<utility::size64_t>(value);
        }

        return headers.content_length();
    }

    archive_status blob_response_parsers::parse_archive_status(const utility::string_t& value)
    {
        if (value == header_value_archive_status_to_hot)
        {
            return archive_status::rehydrate_pending_to_hot;
        }
        else if (value == header_value_archive_status_to_cool)
        {
            return archive_status::rehydrate_pending_to_cool;
        }
        else
        {
            return archive_status::unknown;
        }
    }

    cloud_blob_properties blob_response_parsers::parse_blob_properties(const web::http::http_response& response)
    {
        cloud_blob_properties properties;

        properties.m_etag = parse_etag(response);
        properties.m_last_modified = parse_last_modified(response);
        properties.m_lease_status = parse_lease_status(response);
        properties.m_lease_state = parse_lease_state(response);
        properties.m_lease_duration = parse_lease_duration(response);
        properties.m_size = parse_blob_size(response);

        auto& headers = response.headers();
        properties.m_page_blob_sequence_number = utility::conversions::details::scan_string<int64_t>(get_header_value(headers, ms_header_blob_sequence_number));
        properties.m_append_blob_committed_block_count = utility::conversions::details::scan_string<int>(get_header_value(headers, ms_header_blob_committed_block_count));
        properties.m_cache_control = get_header_value(headers, web::http::header_names::cache_control);
        properties.m_content_disposition = get_header_value(headers, header_content_disposition);
        properties.m_content_encoding = get_header_value(headers, web::http::header_names::content_encoding);
        properties.m_content_language = get_header_value(headers, web::http::header_names::content_language);
        properties.m_content_type = get_header_value(headers, web::http::header_names::content_type);
        properties.m_type = parse_blob_type(get_header_value(headers, ms_header_blob_type));
        // When content_range is not empty, it means the request is Get Blob with range specified, then 'Content-MD5' header should not be used.
        properties.m_content_md5 = get_header_value(headers, ms_header_blob_content_md5);
        if (properties.m_content_md5.empty() && get_header_value(headers, web::http::header_names::content_range).empty())
        {
            properties.m_content_md5 = get_header_value(headers, web::http::header_names::content_md5);
        }
        
        auto change_time_string = get_header_value(headers, ms_header_tier_change_time);
        if (!change_time_string.empty())
        {
            properties.m_access_tier_change_time = utility::datetime::from_string(change_time_string, utility::datetime::date_format::RFC_1123);
        }

        auto tier_string = get_header_value(headers, ms_header_access_tier);
        properties.m_standard_blob_tier = parse_standard_blob_tier(tier_string);
        properties.m_premium_blob_tier = parse_premium_blob_tier(tier_string);
        properties.m_archive_status = parse_archive_status(get_header_value(headers, ms_header_archive_status));
        properties.m_server_encrypted = response_parsers::parse_boolean(get_header_value(headers, ms_header_server_encrypted));
        properties.m_is_incremental_copy = response_parsers::parse_boolean(get_header_value(headers, ms_header_incremental_copy));
        properties.m_access_tier_inferred = response_parsers::parse_boolean(get_header_value(headers, ms_header_access_tier_inferred));

        return properties;
    }

    account_properties blob_response_parsers::parse_account_properties(const web::http::http_response& response)
    {
        account_properties properties;

        properties.m_sku_name = get_header_value(response, protocol::ms_header_sku_name);
        properties.m_account_kind = get_header_value(response, protocol::ms_header_account_kind);

        return properties;
    }

    utility::string_t parse_batch_id(const utility::string_t& content_type_value)
    {
        utility::string_t batch_id;
        static const utility::string_t batch_id_prefix(_XPLATSTR("boundary="));
        size_t batch_id_offset_begin = content_type_value.find(batch_id_prefix) + batch_id_prefix.size();
        batch_id = content_type_value.substr(batch_id_offset_begin);
        return batch_id;
    }

    void parse_set_batch_headers(blob_batch_result& result, const utility::string_t& batch_header_string)
    {
        static const utility::string_t delimiter(_XPLATSTR(": ")), crlf(_XPLATSTR("\r\n"));
        size_t pos = 0, previous_pos = 0;
        while ((pos = batch_header_string.find(crlf, previous_pos)) != utility::string_t::npos)
        {
            //header key starting at previous_pos.
            size_t header_key_end_pos = batch_header_string.find(delimiter, previous_pos);
            // header value ends at pos.
            size_t header_value_start_pos = header_key_end_pos + delimiter.size();
            result.headers()[batch_header_string.substr(previous_pos, header_key_end_pos - previous_pos + 1)] = batch_header_string.substr(header_value_start_pos, pos - header_value_start_pos + 1);
            previous_pos = pos + crlf.size();
        }
    }

    blob_batch_results blob_response_parsers::parse_blob_batch_result(const web::http::http_response& response)
    {
        blob_batch_results results;
        utility::string_t body = response.extract_string().get();


        static const utility::string_t http_begin(_XPLATSTR("HTTP")), server_header(_XPLATSTR("Server: ")), crlf(_XPLATSTR("\r\n")), crlfcrlf(_XPLATSTR("\r\n\r\n"));
        static const utility::string_t double_hypen(_XPLATSTR("--")), space(_XPLATSTR(" "));

        utility::string_t changeset_marker_prefix = double_hypen + parse_batch_id((response.headers().find(utility::string_t(protocol::header_content_type)))->second);
        utility::string_t changeset_begin_marker = changeset_marker_prefix + crlf;

        // Loop through the response looking for changeset begin markers.
        size_t next_changeset = body.find(changeset_begin_marker);
        while (next_changeset != utility::string_t::npos)
        {
            blob_batch_result result;
            // Find the HTTP status line
            size_t response_begin = body.find(http_begin, next_changeset + changeset_begin_marker.size());

            // Find the status code within that line
            size_t status_code_begin = body.find(space, response_begin) + space.size();
            size_t status_code_end = body.find(space, status_code_begin);
            utility::string_t status_code_string = body.substr(status_code_begin, status_code_end - status_code_begin + 1);

            // Extract the status code as an web::http::status_code
            result.set_status_code(utility::conversions::details::scan_string<web::http::status_code>(status_code_string));

            //parse the headers
            //header will always end with "Server: <Header Value>, header value\r\n" e.g. Windows-Azure-Blob/1.0
            size_t header_begin = body.find(crlf, status_code_end + status_code_string.size()) + crlf.size();
            size_t server_header_begin = body.find(server_header, header_begin);
            size_t header_end = body.find(crlf, server_header_begin) + crlf.size();
            parse_set_batch_headers(result, body.substr(header_begin, header_end - header_begin + 1));

            next_changeset = body.find(changeset_begin_marker, header_end);
            result.set_body(body.substr(header_end, next_changeset - header_end + 1 - crlf.size()));
            results.batch_results().push_back(result);
        }
        return results;
    }
}}} // namespace azure::storage::protocol
