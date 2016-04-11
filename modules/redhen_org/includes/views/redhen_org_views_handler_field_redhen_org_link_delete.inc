<?php

/**
 * @file
 * Definition of redhen_org_views_handler_field_redhen_org_link_delete.
 */

/**
 * Field handler to present the delete link for a RedHen Organization.
 *
 * @ingroup views_field_handlers
 */
class redhen_org_views_handler_field_redhen_org_link_delete extends redhen_org_views_handler_field_redhen_org_link {

  function render_link($org, $values) {
    if (entity_access('delete', 'redhen_org', $org)) {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = 'redhen/org/' . $org->org_id . '/view/delete';
      $text = !empty($this->options['text']) ? $this->options['text'] : t('Delete');
      return $text;
    }
  }
}
