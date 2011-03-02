(function ($) {


  /**
   * Field instance settings screen: force the 'Display on registration form'
   * checkbox checked whenever 'Required' is checked.
   */
  Drupal.behaviors.redhenFieldsDisplayWidget = {
    attach: function (context, settings) {
      $('.redhen-email-widget-item').each(function(index, element) {
        if ($(element).find('.form-type-textfield input').attr('value') === '') {
          $(element).hide();
        }


      });
    }
  };

  Drupal.redhenFieldsAddItem = function () {
    $('.redhen-email-widget-item').each(function(index, element) {
      if ($(element).is(":visible") == false) {
        $(element).show();
        return false;
      }
    });
  };


})(jQuery);
