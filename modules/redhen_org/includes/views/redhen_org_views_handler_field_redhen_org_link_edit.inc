<?php

/**
 * @file
 * Definition of redhen_org_views_handler_field_redhen_org_link_edit.
 */

/**
 * Field handler to present the edit link for a RedHen Organization.
 *
 * @ingroup views_field_handlers
 */
class redhen_org_views_handler_field_redhen_org_link_edit extends redhen_org_views_handler_field_redhen_org_link {

  function render_link($org, $values) {
    if (entity_access('edit', 'redhen_org', $org)) {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = 'redhen/org/' . $org->org_id . '/view/edit';
      $text = !empty($this->options['text']) ? $this->options['text'] : t('Edit');
      return $text;
    }
  }
}
