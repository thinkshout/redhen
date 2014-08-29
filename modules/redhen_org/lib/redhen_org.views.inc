<?php

/**
 * Controls views integration for Redhen organizations
 */
class RedhenOrgViewsController extends EntityDefaultViewsController {

  /**
   * Defines the result for hook_views_data().
   */
  public function views_data() {
    $data = parent::views_data();

    // View link.
    $data['redhen_org']['link'] = array(
      'title' => t('Link'),
      'help' => t('Provide a simple link to the RedHen Organization.'),
      'field' => array(
        'handler' => 'redhen_org_views_handler_field_redhen_org_link',
        'click sortable' => TRUE,
        'real field' => 'org_id',
        'additional fields' => array(
          'org_id',
        ),
      ),
    );

    // Edit link.
    $data['redhen_org']['edit'] = array(
      'title' => t('Edit link'),
      'help' => t('Provide a simple link to edit the RedHen Organization.'),
      'field' => array(
        'handler' => 'redhen_org_views_handler_field_redhen_org_link_edit',
        'click sortable' => TRUE,
        'real field' => 'org_id',
        'additional fields' => array(
          'org_id',
        ),
      ),
    );

    // Delete link.
    $data['redhen_org']['delete'] = array(
      'title' => t('Delete link'),
      'help' => t('Provide a simple link to delete the RedHen Organization.'),
      'field' => array(
        'handler' => 'redhen_org_views_handler_field_redhen_org_link_delete',
        'click sortable' => TRUE,
        'real field' => 'org_id',
        'additional fields' => array(
          'org_id',
        ),
      ),
    );

    return $data;
  }

}
