<?php
/**
 * @file
 * Redhen Group entity class
 */

/**
 * The class used for group entities.
 */
class RedhenOrg extends Entity {

  public function __construct(array $values = array(), $entityType = NULL) {
    parent::__construct($values, $entityType);
  }

  /**
   * Override buildContent() to add contact properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    $content['name'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Label'),
      '#items' => array(
        array(
          '#markup' => check_plain($this->label)
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    return entity_get_controller($this->entityType)
      ->buildContent($this, $view_mode, $langcode, $content);
  }

  /**
   * Specifies the default label, which is picked up by label() by default.
   */
  protected function defaultLabel() {
    $type = redhen_org_get_types($this->type);
    return $type->label . ': ' . $this->label;
  }

}
