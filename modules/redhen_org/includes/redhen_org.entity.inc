<?php
/**
 * @file
 * Redhen Group entity classes
 */

/**
 * The class used for group entities.
 */
class RedhenOrg extends Entity {

  public function __construct(array $values = array()) {
    parent::__construct($values, 'redhen_org');
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
    $content['disposition'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Disposition'),
      '#items' => array(
        array(
          '#markup' => ($this->disposition == REDHEN_DISPOSITION_ACTIVE) ? t('Active') : t('Inactive')
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

  /**
   * Specifify URI
   */
  protected function defaultUri() {
    return array('path' => 'redhen/org/' . $this->identifier());
  }

}

/**
 * Controls metadata for Redhen organizations
 */
class RedhenOrgMetadataController extends EntityDefaultMetadataController {
  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $properties['created'] = array(
      'label' => t("Created"),
      'description' => t("The date the organization was created."),
      'type' => 'date',
      'schema field' => 'created',
    );
    $properties['updated'] = array(
      'label' => t("Updated"),
      'description' => t("The date the organization was updated."),
      'type' => 'date',
      'schema field' => 'updated',
    );
    $properties['author'] = array(
      'label' => t("Author"),
      'type' => 'user',
      'description' => t("The author of the organization record."),
      'schema field' => 'author_uid'
    );

    return $info;
  }
}

