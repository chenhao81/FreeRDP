/**
 * FreeRDP: A Remote Desktop Protocol Client
 * Windowing Alternate Secondary Orders
 *
 * Copyright 2011 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 * Copyright 2011 Roman Barabanov <romanbarabanov@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <freerdp/utils/rail.h>
#include <freerdp/utils/memory.h>

#include "window.h"

void update_read_icon_info(STREAM* s, ICON_INFO* icon_info)
{
	stream_read_uint16(s, icon_info->cacheEntry); /* cacheEntry (2 bytes) */
	stream_read_uint8(s, icon_info->cacheId); /* cacheId (1 byte) */
	stream_read_uint8(s, icon_info->bpp); /* bpp (1 byte) */
	stream_read_uint16(s, icon_info->width); /* width (2 bytes) */
	stream_read_uint16(s, icon_info->height); /* height (2 bytes) */
	stream_read_uint16(s, icon_info->cbColorTable); /* cbColorTable (2 bytes) */
	stream_read_uint16(s, icon_info->cbBitsMask); /* cbBitsMask (2 bytes) */
	stream_read_uint16(s, icon_info->cbBitsColor); /* cbBitsColor (2 bytes) */

	/* bitsMask */
	if (icon_info->bitsMask == NULL)
		icon_info->bitsMask = (uint8*) xmalloc(icon_info->cbBitsMask);
	else
		icon_info->bitsMask = (uint8*) xrealloc(icon_info->bitsMask, icon_info->cbBitsMask);
	stream_read(s, icon_info->bitsMask, icon_info->cbBitsMask);

	/* colorTable */
	if (icon_info->colorTable == NULL)
		icon_info->colorTable = (uint8*) xmalloc(icon_info->cbColorTable);
	else
		icon_info->colorTable = (uint8*) xrealloc(icon_info->colorTable, icon_info->cbColorTable);
	stream_read(s, icon_info->colorTable, icon_info->cbColorTable);

	/* bitsColor */
	if (icon_info->bitsColor == NULL)
		icon_info->bitsColor = (uint8*) xmalloc(icon_info->cbBitsColor);
	else
		icon_info->bitsColor = (uint8*) xrealloc(icon_info->bitsColor, icon_info->cbBitsColor);
	stream_read(s, icon_info->bitsColor, icon_info->cbBitsColor);
}

void update_read_cached_icon_info(STREAM* s, CACHED_ICON_INFO* cached_icon_info)
{
	stream_read_uint16(s, cached_icon_info->cacheEntry); /* cacheEntry (2 bytes) */
	stream_read_uint8(s, cached_icon_info->cacheId); /* cacheId (1 byte) */
}

void update_read_notify_icon_infotip(STREAM* s, NOTIFY_ICON_INFOTIP* notify_icon_infotip)
{
	stream_read_uint32(s, notify_icon_infotip->timeout); /* timeout (4 bytes) */
	stream_read_uint32(s, notify_icon_infotip->flags); /* infoFlags (4 bytes) */
	rail_read_unicode_string(s, &notify_icon_infotip->text); /* infoTipText */
	rail_read_unicode_string(s, &notify_icon_infotip->title); /* title */
}

void update_read_window_state_order(STREAM* s, WINDOW_ORDER_INFO* orderInfo, WINDOW_STATE_ORDER* window_state)
{
	int i;
	int size;

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_OWNER)
		stream_read_uint32(s, window_state->ownerWindowId); /* ownerWindowId (4 bytes) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_STYLE)
	{
		stream_read_uint32(s, window_state->style); /* style (4 bytes) */
		stream_read_uint32(s, window_state->extendedStyle); /* extendedStyle (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_SHOW)
		stream_read_uint8(s, window_state->showState); /* showState (1 byte) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_TITLE)
		rail_read_unicode_string(s, &window_state->titleInfo); /* titleInfo */

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_OFFSET)
	{
		stream_read_uint32(s, window_state->clientOffsetX); /* clientOffsetX (4 bytes) */
		stream_read_uint32(s, window_state->clientOffsetY); /* clientOffsetY (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_CLIENT_AREA_SIZE)
	{
		stream_read_uint32(s, window_state->clientAreaWidth); /* clientAreaWidth (4 bytes) */
		stream_read_uint32(s, window_state->clientAreaHeight); /* clientAreaHeight (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_RP_CONTENT)
		stream_read_uint8(s, window_state->RPContent); /* RPContent (1 byte) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_ROOT_PARENT)
		stream_read_uint32(s, window_state->rootParentHandle);/* rootParentHandle (4 bytes) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_OFFSET)
	{
		stream_read_uint32(s, window_state->windowOffsetX); /* windowOffsetX (4 bytes) */
		stream_read_uint32(s, window_state->windowOffsetY); /* windowOffsetY (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_CLIENT_DELTA)
	{
		stream_read_uint32(s, window_state->windowClientDeltaX); /* windowClientDeltaX (4 bytes) */
		stream_read_uint32(s, window_state->windowClientDeltaY); /* windowClientDeltaY (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_SIZE)
	{
		stream_read_uint32(s, window_state->windowWidth); /* windowWidth (4 bytes) */
		stream_read_uint32(s, window_state->windowHeight); /* windowHeight (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_WND_RECTS)
	{
		stream_read_uint16(s, window_state->numWindowRects); /* numWindowRects (2 bytes) */

		size = sizeof(RECTANGLE_16) * window_state->numWindowRects;

		if (window_state->windowRects == NULL)
			window_state->windowRects = (RECTANGLE_16*) xmalloc(size);
		else
			window_state->windowRects = (RECTANGLE_16*) xrealloc(window_state->windowRects, size);

		/* windowRects */
		for (i = 0; i < window_state->numWindowRects; i++)
		{
			rail_read_rectangle_16(s, &window_state->windowRects[i]);
		}
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VIS_OFFSET)
	{
		stream_read_uint32(s, window_state->visibleOffsetX); /* visibleOffsetX (4 bytes) */
		stream_read_uint32(s, window_state->visibleOffsetY); /* visibleOffsetY (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_VISIBILITY)
	{
		stream_read_uint16(s, window_state->numVisibilityRects); /* numVisibilityRects (2 bytes) */

		size = sizeof(RECTANGLE_16) * window_state->numVisibilityRects;

		if (window_state->visibilityRects == NULL)
			window_state->visibilityRects = (RECTANGLE_16*) xmalloc(size);
		else
			window_state->visibilityRects = (RECTANGLE_16*) xrealloc(window_state->visibilityRects, size);

		/* visibilityRects */
		for (i = 0; i < window_state->numVisibilityRects; i++)
		{
			rail_read_rectangle_16(s, &window_state->visibilityRects[i]);
		}
	}
}

void update_read_window_icon_order(STREAM* s, WINDOW_ORDER_INFO* orderInfo, WINDOW_ICON_ORDER* window_icon)
{
	update_read_icon_info(s, &window_icon->iconInfo); /* iconInfo (ICON_INFO) */
}

void update_read_window_cached_icon_order(STREAM* s, WINDOW_ORDER_INFO* orderInfo, WINDOW_CACHED_ICON_ORDER* window_cached_icon)
{
	update_read_cached_icon_info(s, &window_cached_icon->cachedIcon); /* cachedIcon (CACHED_ICON_INFO) */
}

void update_read_window_deleted_order(STREAM* s, WINDOW_ORDER_INFO* orderInfo)
{
	/* window deletion event */
}

void update_recv_window_info_order(rdpUpdate* update, STREAM* s, WINDOW_ORDER_INFO* orderInfo)
{
	stream_read_uint32(s, orderInfo->windowId); /* windowId (4 bytes) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_ICON)
	{
		update_read_window_icon_order(s, orderInfo, &update->window_icon);
		IFCALL(update->WindowIcon, update, orderInfo, &update->window_icon);
	}
	else if (orderInfo->fieldFlags & WINDOW_ORDER_CACHED_ICON)
	{
		update_read_window_cached_icon_order(s, orderInfo, &update->window_cached_icon);
		IFCALL(update->WindowCachedIcon, update, orderInfo, &update->window_cached_icon);
	}
	else if (orderInfo->fieldFlags & WINDOW_ORDER_STATE_DELETED)
	{
		update_read_window_deleted_order(s, orderInfo);
		IFCALL(update->WindowDeleted, update, orderInfo);
	}
	else
	{
		update_read_window_state_order(s, orderInfo, &update->window_state);
		IFCALL(update->WindowState, update, orderInfo, &update->window_state);
	}
}

void update_read_notification_icon_state_order(STREAM* s, WINDOW_ORDER_INFO* orderInfo, NOTIFY_ICON_STATE_ORDER* notify_icon_state)
{
	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_VERSION)
		stream_read_uint32(s, notify_icon_state->version); /* version (4 bytes) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_TIP)
		rail_read_unicode_string(s, &notify_icon_state->toolTip); /* toolTip (UNICODE_STRING) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_INFO_TIP)
		update_read_notify_icon_infotip(s, &notify_icon_state->infoTip); /* infoTip (NOTIFY_ICON_INFOTIP) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_STATE)
		stream_read_uint32(s, notify_icon_state->state); /* state (4 bytes) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_ICON)
		update_read_icon_info(s, &notify_icon_state->icon); /* icon (ICON_INFO) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_CACHED_ICON)
		update_read_cached_icon_info(s, &notify_icon_state->cachedIcon); /* cachedIcon (CACHED_ICON_INFO) */
}

void update_read_notification_icon_deleted_order(STREAM* s, WINDOW_ORDER_INFO* orderInfo)
{
	/* notification icon deletion event */
}

void update_recv_notification_icon_info_order(rdpUpdate* update, STREAM* s, WINDOW_ORDER_INFO* orderInfo)
{
	stream_read_uint32(s, orderInfo->windowId); /* windowId (4 bytes) */
	stream_read_uint32(s, orderInfo->notifyIconId); /* notifyIconId (4 bytes) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_STATE_DELETED)
	{
		update_read_notification_icon_deleted_order(s, orderInfo);
		IFCALL(update->NotifyIconDeleted, update, orderInfo);
	}
	else
	{
		update_read_notification_icon_state_order(s, orderInfo, &update->notify_icon_state);
		IFCALL(update->NotifyIconState, update, orderInfo, &update->notify_icon_state);
	}
}

void update_read_desktop_actively_monitored_order(STREAM* s, WINDOW_ORDER_INFO* orderInfo, MONITORED_DESKTOP_ORDER* monitored_desktop)
{
	int i;
	int size;

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_DESKTOP_ACTIVE_WND)
		stream_read_uint32(s, monitored_desktop->activeWindowId); /* activeWindowId (4 bytes) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_DESKTOP_ZORDER)
	{
		stream_read_uint8(s, monitored_desktop->numWindowIds); /* numWindowIds (1 byte) */

		size = sizeof(uint32) * monitored_desktop->numWindowIds;

		if (monitored_desktop->windowIds == NULL)
			monitored_desktop->windowIds = (uint32*) xmalloc(size);
		else
			monitored_desktop->windowIds = (uint32*) xrealloc(monitored_desktop->windowIds, size);

		/* windowIds */
		for (i = 0; i < monitored_desktop->numWindowIds; i++)
		{
			stream_read_uint32(s, monitored_desktop->windowIds[i]);
		}
	}
}

void update_read_desktop_non_monitored_order(STREAM* s, WINDOW_ORDER_INFO* orderInfo)
{
	/* non-monitored desktop notification event */
}

void update_recv_desktop_info_order(rdpUpdate* update, STREAM* s, WINDOW_ORDER_INFO* orderInfo)
{
	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_DESKTOP_NONE)
	{
		update_read_desktop_non_monitored_order(s, orderInfo);
		IFCALL(update->NonMonitoredDesktop, update, orderInfo);
	}
	else
	{
		update_read_desktop_actively_monitored_order(s, orderInfo, &update->monitored_desktop);
		IFCALL(update->MonitoredDesktop, update, orderInfo, &update->monitored_desktop);
	}
}

void update_recv_altsec_window_order(rdpUpdate* update, STREAM* s)
{
	uint16 orderSize;

	stream_read_uint16(s, orderSize); /* orderSize (2 bytes) */
	stream_read_uint32(s, update->orderInfo.fieldFlags); /* FieldsPresentFlags (4 bytes) */

	if (update->orderInfo.fieldFlags & WINDOW_ORDER_TYPE_WINDOW)
		update_recv_window_info_order(update, s, &update->orderInfo);
	else if (update->orderInfo.fieldFlags & WINDOW_ORDER_TYPE_NOTIFY)
		update_recv_notification_icon_info_order(update, s, &update->orderInfo);
	else if (update->orderInfo.fieldFlags & WINDOW_ORDER_TYPE_DESKTOP)
		update_recv_desktop_info_order(update, s, &update->orderInfo);
}

